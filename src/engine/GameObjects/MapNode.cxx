#include <cstdlib>
#include "MapNode.hxx"

#include "LOG.hxx"
#include "../map/MapLayers.hxx"
#include "GameStates.hxx"
#include "Settings.hxx"

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
  const Layer layer = TileManager::instance().getTileLayer(tileID);
  updateTexture(layer);
}

bool MapNode::changeHeight(const bool higher)
{
  constexpr int minHeight = 0;
  auto &height = m_isoCoordinates.height;

  if ((higher && (height < maxHeight)) || (!higher && (height > minHeight)))
  {
    higher ? ++height : --height;
    m_sprite->isoCoordinates = m_isoCoordinates;
    return true;
  }

  return false;
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
    switch (layer)
    {
    case Layer::ZONE:
      this->setNodeTransparency(Settings::instance().zoneLayerTransparency, Layer::ZONE);
      break;
    case Layer::WATER:
      demolishLayer(Layer::ROAD);
      //TODO: we need to modify neighbors TileTypes to Shore.
      // no break on purpose.
    case Layer::ROAD:
      // in case it's allowed then maybe a Tree Tile already exist, so we remove it.
      demolishLayer(Layer::BUILDINGS);
      break;
    case Layer::BUILDINGS:
      if (tileData->category != "Flora")
      {
        this->setNodeTransparency(0.6, Layer::BLUEPRINT);
      }
      m_mapNodeData[Layer::ZONE].shouldRender = false;
      break;
    default:
      break;
    }

    if (isLayerOccupied(Layer::ZONE))
    {
      // TODO: check decorations.
      if (tileData->category != "Flora" && tileData->category != m_mapNodeData[Layer::ZONE].tileData->subCategory)
      {
        // selected tile category != existed zone category.
        demolishLayer(Layer::ZONE);
      }
    }
    m_mapNodeData[layer].origCornerPoint = origCornerPoint;
    m_previousTileID = m_mapNodeData[layer].tileID;
    m_mapNodeData[layer].tileData = tileData;
    m_mapNodeData[layer].tileID = tileID;

    // Determine if the tile should have a random rotation or not.
    if (m_mapNodeData[layer].tileData->tiles.pickRandomTile && m_mapNodeData[layer].tileData->tiles.count > 1)
    {
      /** set tileIndex to a rand between 1 and count, this will be the displayed image of the entire tileset
      * if this tile has ordered frames, like roads then pickRandomTile must be set to 0.
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
    updateTexture(layer);
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
  else if (MapLayers::isLayerActive(Layer::GROUND_DECORATION) && m_mapNodeData[Layer::GROUND_DECORATION].tileData)
  {
    return Layer::GROUND_DECORATION;
  }
  // terrain is our fallback, since there's always terrain.
  else if (MapLayers::isLayerActive(Layer::TERRAIN) && m_mapNodeData[Layer::TERRAIN].tileData)
  {
    return Layer::TERRAIN;
  }
  return Layer::NONE;
}

void MapNode::setNodeTransparency(const float transparencyFactor, const Layer &layer) const
{
  // TODO refactoring: Consider replacing magic number (255) with constexpr.
  unsigned char alpha = (1 - transparencyFactor) * 255;
  m_sprite->setSpriteTranparencyFactor(layer, alpha);
}

bool MapNode::isDataAutoTile(const TileData *tileData)
{
  if (tileData)
  {
    if (tileData->tileType == +TileType::ROAD || tileData->tileType == +TileType::AUTOTILE ||
        tileData->tileType == +TileType::UNDERGROUND)
    {
      return true;
    }
  }
  return false;
}

bool MapNode::isLayerAutoTile(const Layer &layer) const { return isDataAutoTile(m_mapNodeData[layer].tileData); }

bool MapNode::isPlacableOnSlope(const std::string &tileID) const
{
  TileData *tileData = TileManager::instance().getTileData(tileID);
  if (tileData && tileData->tileType == +TileType::ZONE)
  {
    // zones are allowed to pass slopes.
    return true;
  }
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
    switch (layer)
    {
    case Layer::ROAD:
      if (isLayerOccupied(Layer::ROAD))
      {
        return true;
      }
      else if ((isLayerOccupied(Layer::BUILDINGS) && (m_mapNodeData[Layer::BUILDINGS].tileData->category != "Flora")) ||
               isLayerOccupied(Layer::WATER) || !isPlacableOnSlope(newTileID))
      {
        return false;
      }
    case Layer::BUILDINGS:
      if (isLayerOccupied(Layer::ROAD))
      {
        return false;
      }
    }
    //this is a water tile and placeOnWater has not been set to true, building is not permitted. Also disallow placing of water tiles on non water tiles
    if (tileData->tileType != +TileType::WATER &&
        (isLayerOccupied(Layer::WATER) && !tileData->placeOnWater || !isLayerOccupied(Layer::WATER) && tileData->placeOnWater))
    {
      return false;
    }

    // check if the current tile has the property overplacable set or if it's of the same tile ID for certain TileTypes only (not DEFAULT)
    if (m_mapNodeData[layer].tileData &&
        (m_mapNodeData[layer].tileData->tileType == +TileType::GROUNDDECORATION ||
         m_mapNodeData[layer].tileData->isOverPlacable ||
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

void MapNode::updateTexture(const Layer &layer)
{
  SDL_Rect clipRect{0, 0, 0, 0};
  //TODO: Refactor this
  m_elevationOrientation = TileManager::instance().calculateSlopeOrientation(m_elevationBitmask);
  std::vector<Layer> layersToGoOver;
  if (layer != Layer::NONE)
  {
    // in case this is not the default value (which is NONE), we need to update only 1 layer.
    layersToGoOver.push_back(layer);
  }
  else
  {
    layersToGoOver.insert(layersToGoOver.begin(), std::begin(allLayersOrdered), std::end(allLayersOrdered));
  }

  for (auto currentLayer : layersToGoOver)
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
            // for shore tiles, we need to reset the tileIndex to 0, else a random tile would be picked. This is a little bit hacky.
            m_mapNodeData[Layer::TERRAIN].tileIndex = 0;
          }
        }
        // if the node should autotile, check if it needs to tile itself to another tile of the same ID
        else if (isLayerAutoTile(currentLayer) && (this->getTileID(currentLayer) == m_mapNodeData[currentLayer].tileID))
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
          // only check for rectangular roads when there are frames for it. Spritesheets with rect-roads have 20 items
          if (GameStates::instance().rectangularRoads && m_mapNodeData[currentLayer].tileData->tiles.count == 20)
          {
            switch (m_autotileOrientation[currentLayer])
            {
            case TileOrientation::TILE_S_AND_W:
              m_autotileOrientation[currentLayer] = TileOrientation::TILE_S_AND_W_RECT;
              break;
            case TileOrientation::TILE_S_AND_E:
              m_autotileOrientation[currentLayer] = TileOrientation::TILE_S_AND_E_RECT;
              break;
            case TileOrientation::TILE_N_AND_E:
              m_autotileOrientation[currentLayer] = TileOrientation::TILE_N_AND_E_RECT;
              break;
            case TileOrientation::TILE_N_AND_W:
              m_autotileOrientation[currentLayer] = TileOrientation::TILE_N_AND_W_RECT;
              break;

            default:
              break;
            }
          }
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
        updateTexture(currentLayer);
      }
      m_sprite->spriteCount = spriteCount;
    }
  }
}

bool MapNode::isSlopeNode() const { return m_mapNodeData[Layer::TERRAIN].tileMap == TileMap::SLOPES; }

void MapNode::setCoordinates(const Point &newIsoCoordinates)
{
  m_isoCoordinates = newIsoCoordinates;
  m_sprite->isoCoordinates = m_isoCoordinates;
}

const MapNodeData &MapNode::getActiveMapNodeData() const { return m_mapNodeData[getTopMostActiveLayer()]; }

void MapNode::setMapNodeData(std::vector<MapNodeData> &&mapNodeData, const Point &currNodeIsoCoordinates)
{
  m_mapNodeData.swap(mapNodeData);
  this->setNodeTransparency(Settings::instance().zoneLayerTransparency, Layer::ZONE);

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

void MapNode::demolishLayer(const Layer &layer)
{
  m_mapNodeData[layer].tileData = nullptr;
  m_mapNodeData[layer].tileID = "";
  m_autotileOrientation[layer] =
      TileOrientation::TILE_DEFAULT_ORIENTATION; // We need to reset TileOrientation, in case it's set (demolishing autotiles)
  m_mapNodeData[layer].origCornerPoint = this->getCoordinates();
  m_mapNodeData[Layer::ZONE].shouldRender = true;
  m_sprite->clearSprite(layer);
}

void MapNode::demolishNode(const Layer &demolishLayer)
{
  // allow to delete a single layer only
  std::vector<Layer> layersToDemolish;
  if (demolishLayer == Layer::NONE)
  {
    layersToDemolish = {Layer::BUILDINGS, Layer::UNDERGROUND, Layer::GROUND_DECORATION, Layer::ZONE, Layer::ROAD};
  }
  else
  {
    layersToDemolish.push_back(demolishLayer);
  }

  for (auto &layer : layersToDemolish)
  {
    if (MapLayers::isLayerActive(layer) && m_mapNodeData[layer].tileData)
    {
      if ((GameStates::instance().demolishMode == DemolishMode::DEFAULT &&
           m_mapNodeData[layer].tileData->tileType == +TileType::ZONE) ||
          (GameStates::instance().demolishMode == DemolishMode::DE_ZONE &&
           m_mapNodeData[layer].tileData->tileType != +TileType::ZONE) ||
          (GameStates::instance().demolishMode == DemolishMode::GROUND_DECORATION &&
           m_mapNodeData[layer].tileData->tileType != +TileType::GROUNDDECORATION))
      {
        continue;
      }
      this->demolishLayer(layer);
      if (layer == Layer::BUILDINGS)
      {
        this->setNodeTransparency(0, Layer::BLUEPRINT);
      }
      updateTexture(demolishLayer);
    }
  }
}
