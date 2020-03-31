#include <cstdlib>
#include "MapNode.hxx"

#include "LOG.hxx"
#include "../map/MapLayers.hxx"

MapNode::MapNode(Point isoCoordinates, const std::string &terrainID, const std::string &tileID)
    : m_isoCoordinates(std::move(isoCoordinates))
{
  m_mapNodeData.resize(LAYERS_COUNT);
  m_autotileBitmask.resize(LAYERS_COUNT);
  m_autotileOrientation.resize(LAYERS_COUNT);
  m_sprite = std::make_unique<Sprite>(m_isoCoordinates);

  //initialize vectors
  for (auto &it : m_autotileBitmask)
  {
    it = 0;
  }
  for (auto &it : m_autotileOrientation)
  {
    it = TileOrientation::TILE_DEFAULT_ORIENTATION;
  }
  for (auto &it : m_mapNodeData)
  {
    it.origCornerPoint = isoCoordinates;
  }

  setTileID(terrainID, isoCoordinates);
  if (!tileID.empty()) // in case tileID is not supplied skip it
  {
    setTileID(tileID, isoCoordinates);
  }
  // always add blueprint tiles too when creating the node
  setTileID("terrain_blueprint", isoCoordinates);

  updateTexture();
}

void MapNode::increaseHeight()
{
  const int height = m_isoCoordinates.height;

  if (height < m_maxHeight)
  {
    m_isoCoordinates.height++;
    m_sprite->isoCoordinates = m_isoCoordinates;
  }
}

void MapNode::decreaseHeight()
{
  const int height = m_isoCoordinates.height;

  if (height > 0)
  {
    m_isoCoordinates.height--;
    m_sprite->isoCoordinates = m_isoCoordinates;
  }
}

void MapNode::render() const { m_sprite->render(); }

void MapNode::setBitmask(unsigned char elevationBitmask, std::vector<uint8_t> autotileBitmask)
{
  m_elevationBitmask = elevationBitmask;
  m_autotileBitmask = autotileBitmask;
  updateTexture();
}

void MapNode::setTileID(const std::string &tileID, const Point &origCornerPoint)
{
  TileData *tileData = TileManager::instance().getTileData(tileID);
  if (tileData && !tileID.empty())
  {
    const Layer layer = TileManager::instance().getTileLayer(tileID);
    m_mapNodeData[layer].origCornerPoint = origCornerPoint;
    m_previousTileID = m_mapNodeData[layer].tileID;
    m_mapNodeData[layer].tileData = tileData;
    m_mapNodeData[layer].tileID = tileID;

    // Determine if the tile should have a random rotation or not.
    if (m_mapNodeData[layer].tileData->tiles.rotations <= 1)
    {
      /** set tileIndex to a rand between 1 and count, this will be the displayed image
      * if this tile has any purposefully set rotation images (roads and buildings
      * with rotations) then always set tileIndex to 0.
      **/
      m_mapNodeData[layer].tileIndex = rand() % m_mapNodeData[layer].tileData->tiles.count;
    }
    else
    {
      /** must be reset to 0 otherwise overwritting tiles would keep the old
      * tile's tileIndex which creates problems if it's supposed to be 0
      **/
      m_mapNodeData[layer].tileIndex = 0;
    }
    updateTexture();
  }
}

Layer MapNode::getTopMostActiveLayer() const
{
  if (MapLayers::isLayerActive(Layer::BUILDINGS) && m_mapNodeData[Layer::BUILDINGS].tileData)
  {
    return Layer::BUILDINGS;
  }
  else if (MapLayers::isLayerActive(Layer::BLUEPRINT) && m_mapNodeData[Layer::UNDERGROUND].tileData)
  {
    return Layer::UNDERGROUND;
  }
  else if (MapLayers::isLayerActive(Layer::BLUEPRINT) && m_mapNodeData[Layer::BLUEPRINT].tileData)
  {
    return Layer::BLUEPRINT;
  }
  // terrain is our fallback, since there's always terrain.
  else if (MapLayers::isLayerActive(Layer::TERRAIN) && m_mapNodeData[Layer::TERRAIN].tileData)
  {
    return Layer::TERRAIN;
  }
  return Layer::NONE;
}

bool MapNode::isPlacableOnSlope(const std::string &tileID) const
{
  TileData *tileData = TileManager::instance().getTileData(tileID);
  if (tileData && m_elevationOrientation != TileSlopes::DEFAULT_ORIENTATION)
  {
    // we need to check the terrain layer for it's orientation so we can calculate the resulting x offset in the spritesheet.
    const int clipRectX = tileData->slopeTiles.clippingWidth * static_cast<int>(m_autotileOrientation[Layer::TERRAIN]);
    // while loading game, m_previousTileID will be equal to "terrain" for terrin tiles while it's empty "" when starting new game.
    // so the check here on m_previousTileID is needed both (temporary), empty and "terrain", this will be fixed in new PR.
    if (clipRectX >= static_cast<int>(tileData->slopeTiles.count) * tileData->slopeTiles.clippingWidth &&
        (m_previousTileID.empty() || m_previousTileID == "terrain"))
    {
      return false;
    }
  }
  return true;
}

bool MapNode::isPlacementAllowed(const std::string &newTileID) const
{
  TileData *tileData = TileManager::instance().getTileData(newTileID);
  const Layer layer = TileManager::instance().getTileLayer(newTileID);

  if (tileData)
  {
    //this is a water tile and placeOnWater has not been set to true, building is not permitted. Also disallow placing of water tiles on non water tiles
    if (tileData->tileType != +TileType::WATER && (m_mapNodeData[Layer::WATER].tileData && !tileData->placeOnWater ||
                                                   !m_mapNodeData[Layer::WATER].tileData && tileData->placeOnWater))
    {
      return false;
    }

    // check if the current tile has the property overplacable set or if it's of the same tile ID for certain TileTypes only (not DEFAULT)
    if (m_mapNodeData[layer].tileData &&
        (m_mapNodeData[layer].tileData->isOverPlacable ||
         (m_mapNodeData[layer].tileData->tileType != +TileType::DEFAULT && m_mapNodeData[layer].tileID == newTileID)))
    {
      return true;
    }

    return isPlacableOnSlope(newTileID) &&
           (m_mapNodeData[layer].tileID.empty() || m_mapNodeData[layer].tileData->tileType == +TileType::TERRAIN ||
            m_mapNodeData[layer].tileData->tileType == +TileType::BLUEPRINT);
  }

  return false;
}

void MapNode::updateTexture()
{
  SDL_Rect clipRect{0, 0, 0, 0};
  //TODO: Refactor this
  m_elevationOrientation = TileManager::instance().calculateSlopeOrientation(m_elevationBitmask);

  for (auto currentLayer : allLayersOrdered)
  {
    if (m_mapNodeData[currentLayer].tileData)
    {
      size_t spriteCount = 1;
      m_mapNodeData[currentLayer].tileMap = TileMap::DEFAULT;
      if (m_elevationOrientation == TileSlopes::DEFAULT_ORIENTATION)
      {
        if (m_mapNodeData[currentLayer].tileData->tileType == +TileType::WATER ||
            m_mapNodeData[currentLayer].tileData->tileType == +TileType::TERRAIN ||
            m_mapNodeData[currentLayer].tileData->tileType == +TileType::BLUEPRINT)
        {
          m_autotileOrientation[currentLayer] = TileManager::instance().calculateTileOrientation(m_autotileBitmask[currentLayer]);
          m_mapNodeData[currentLayer].tileMap = TileMap::DEFAULT;
          if (currentLayer == Layer::TERRAIN && m_autotileOrientation[currentLayer] != TileOrientation::TILE_DEFAULT_ORIENTATION)
          {
            m_mapNodeData[Layer::TERRAIN].tileMap = TileMap::SHORE;
            // for shoretiles, we need to reset the tileIndex to 0, else a random tile would be picked. This is a a litlte bit hacky.
            m_mapNodeData[Layer::TERRAIN].tileIndex = 0;
          }
        }
        // if the node should autotile, check if it needs to tile itself to another tile of the same ID
        else if (m_mapNodeData[currentLayer].tileData->tileType == +TileType::AUTOTILE ||
                 m_mapNodeData[currentLayer].tileData->tileType == +TileType::UNDERGROUND)
        {
          m_autotileOrientation[currentLayer] = TileManager::instance().calculateTileOrientation(m_autotileBitmask[currentLayer]);
        }
      }
      else if (m_elevationOrientation >= TileSlopes::N && m_elevationOrientation <= TileSlopes::BETWEEN)
      {
        if (m_mapNodeData[currentLayer].tileData->slopeTiles.fileName.empty())
        {
          m_mapNodeData[currentLayer].tileMap = TileMap::DEFAULT;
          m_autotileOrientation[currentLayer] = TileOrientation::TILE_DEFAULT_ORIENTATION;
        }
        else
        {
          m_mapNodeData[currentLayer].tileMap = TileMap::SLOPES; // TileSlopes [N,E,w,S]
          m_autotileOrientation[currentLayer] = static_cast<TileOrientation>(m_elevationOrientation);
        }
      }

      switch (m_mapNodeData[currentLayer].tileMap)
      {
      case TileMap::DEFAULT:
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->tiles.clippingWidth;
        if (m_mapNodeData[currentLayer].tileIndex != 0)
        {
          clipRect.x = m_clippingWidth * m_mapNodeData[currentLayer].tileIndex;
        }
        else
        {
          clipRect.x = m_clippingWidth * static_cast<int>(m_autotileOrientation[currentLayer]);
        }

        if (!m_mapNodeData[currentLayer].tileID.empty())
        {
          m_sprite->setClipRect({clipRect.x + m_clippingWidth * m_mapNodeData[currentLayer].tileData->tiles.offset, 0,
                                 m_clippingWidth, m_mapNodeData[currentLayer].tileData->tiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          if (m_mapNodeData[currentLayer].shouldRender)
          {
            m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID),
                                 static_cast<Layer>(currentLayer));
          }
        }

        spriteCount = m_mapNodeData[currentLayer].tileData->tiles.count;
        break;
      case TileMap::SHORE:
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->shoreTiles.clippingWidth;
        if (m_mapNodeData[currentLayer].tileIndex != 0)
        {
          clipRect.x = m_clippingWidth * m_mapNodeData[currentLayer].tileIndex;
        }
        else
        {
          clipRect.x = m_clippingWidth * static_cast<int>(m_autotileOrientation[currentLayer]);
        }

        if (!m_mapNodeData[currentLayer].tileID.empty())
        {
          m_sprite->setClipRect({clipRect.x + m_clippingWidth * m_mapNodeData[currentLayer].tileData->shoreTiles.offset, 0,
                                 m_clippingWidth, m_mapNodeData[currentLayer].tileData->shoreTiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          if (m_mapNodeData[currentLayer].shouldRender)
          {
            m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID + "_shore"),
                                 static_cast<Layer>(currentLayer));
          }
        }

        spriteCount = m_mapNodeData[currentLayer].tileData->shoreTiles.count;
        break;
      case TileMap::SLOPES:
        if (m_mapNodeData[currentLayer].tileData->slopeTiles.fileName.empty())
        {
          break;
        }
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->slopeTiles.clippingWidth;
        clipRect.x = m_mapNodeData[currentLayer].tileData->slopeTiles.clippingWidth *
                     static_cast<int>(m_autotileOrientation[currentLayer]);
        spriteCount = m_mapNodeData[currentLayer].tileData->slopeTiles.count;
        if (clipRect.x <= static_cast<int>(spriteCount) * m_clippingWidth)
        {
          m_sprite->setClipRect({clipRect.x + m_mapNodeData[currentLayer].tileData->slopeTiles.offset * m_clippingWidth, 0,
                                 m_clippingWidth, m_mapNodeData[currentLayer].tileData->slopeTiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID),
                               static_cast<Layer>(currentLayer));
        }
        break;
      default:
        break;
      }

      if (clipRect.x >= static_cast<int>(spriteCount) * m_clippingWidth)
      {
        m_mapNodeData[currentLayer].tileID = m_previousTileID;
        if (m_previousTileID.empty())
        {
          m_mapNodeData[currentLayer].tileData = nullptr;
        }
        updateTexture();
      }
      m_sprite->spriteCount = spriteCount;
    }
  }
}

void MapNode::setCoordinates(const Point &newIsoCoordinates)
{
  m_isoCoordinates = newIsoCoordinates;
  m_sprite->isoCoordinates = m_isoCoordinates;
}

const MapNodeData &MapNode::getActiveMapNodeData() const { return m_mapNodeData[getTopMostActiveLayer()]; }

void MapNode::setMapNodeData(std::vector<MapNodeData> &&mapNodeData, const Point &currNodeIsoCoordinates)
{
  m_mapNodeData.swap(mapNodeData);

  // updates the pointers to the tiles, after loading tileIDs from json
  for (auto &it : m_mapNodeData)
  {
    delete it.tileData;

    it.tileData = TileManager::instance().getTileData(it.tileID);
    if (it.origCornerPoint != currNodeIsoCoordinates)
    {
      it.shouldRender = false;
    }
  }
}

void MapNode::demolishNode()
{
  Layer myLayers[] = {Layer::BUILDINGS, Layer::UNDERGROUND};
  for (auto &layer : myLayers)
  {
    if (MapLayers::isLayerActive(layer))
    {
      m_mapNodeData[layer].tileData = nullptr;
      m_mapNodeData[layer].tileID = "";
      m_mapNodeData[layer].origCornerPoint = this->getCoordinates();
      m_sprite->clearSprite(layer);
      updateTexture();
      break;
    }
  }
}
