#include <cstdlib>
#include "MapNode.hxx"

#include "LOG.hxx"
#include "../map/MapLayers.hxx"

MapNode::MapNode(Point isoCoordinates, const std::string &terrainID, const std::string &tileID)
    : m_isoCoordinates(std::move(isoCoordinates))
{
  m_mapNodeData.resize(LAYERS_COUNT);
  m_sprite = std::make_unique<Sprite>(m_isoCoordinates);

  setTileID(terrainID, isoCoordinates);
  if (!tileID.empty()) // in case tileID is not supplied skip it
    setTileID(tileID, isoCoordinates);

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

void MapNode::setBitmask(unsigned char elevationBitmask, unsigned char tileIDBitmask)
{
  m_elevationBitmask = elevationBitmask;
  m_tileIDBitmask = tileIDBitmask;
  updateTexture();
}

void MapNode::setTileID(const std::string &tileID, const Point &origCornerPoint)
{
  TileData *tileData = TileManager::instance().getTileData(tileID);
  if (tileData)
  {
    Layer layer = TileManager::instance().getTileLayer(tileID);
    this->m_origCornerPoint = origCornerPoint;
    //m_mapNodeData[layer].origCornerPoint = origCornerPoint;
    m_previousTileID = m_mapNodeData[layer].tileID;
    m_mapNodeData[layer].tileData = tileData;
    m_mapNodeData[layer].tileID = tileID;
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

bool MapNode::isPlacableOnSlope(const std::string &tileID) const
{
  TileData *tileData = TileManager::instance().getTileData(tileID);
  if (tileData && m_elevationOrientation != TileSlopes::DEFAULT_ORIENTATION)
  {
    int clipRectX = tileData->slopeTiles.clippingWidth * static_cast<int>(m_orientation);

    if (clipRectX >= static_cast<int>(tileData->slopeTiles.count) * tileData->slopeTiles.clippingWidth &&
        m_previousTileID.empty())
    {
      return false;
    }
  }
  return true;
}

bool MapNode::isPlacementAllowed(const std::string &newTileID) const
{
  TileData *tileData = TileManager::instance().getTileData(newTileID);
  if (tileData)
  {
    Layer layer = Layer::BUILDINGS;
    if (tileData->tileType == "terrain")
    {
      layer = Layer::TERRAIN;
    }
    //this is a water tile and placeOnWater has not been set to true, building is not permitted. Also disallow placing of water tiles on non water tiles
    if ((m_mapNodeData[Layer::WATER].tileData && !tileData->placeOnWater) ||
        (!m_mapNodeData[Layer::WATER].tileData && tileData->placeOnWater))
    {
      return false;
    }
    std::string previousTileID = newTileID;

    // check if the current tile is overplacable or allow overplacing autotiles if it's of the same tile ID
    if (m_mapNodeData[layer].tileData &&
        (m_mapNodeData[layer].tileData->isOverPlacable ||
         (m_mapNodeData[layer].tileData->tileType == "autotile" && m_mapNodeData[layer].tileID == newTileID)))

    {

      return true;
    }
    return isPlacableOnSlope(newTileID) &&
           (m_mapNodeData[layer].tileID == "" || m_mapNodeData[layer].tileData->tileType == "terrain");
  }
  return false;
}

void MapNode::updateTexture()
{
  SDL_Rect clipRect{0, 0, 0, 0};

  tileMap = TileMap::DEFAULT;
  m_elevationOrientation = TileManager::instance().calculateSlopeOrientation(m_elevationBitmask);

  for (uint32_t currentLayer = 0; currentLayer < LAYERS_COUNT; ++currentLayer)
  {
    if (m_mapNodeData[currentLayer].tileData)
    {
      size_t spriteCount = 1;

      if (m_elevationOrientation == TileSlopes::DEFAULT_ORIENTATION)
      {
        if (m_mapNodeData[currentLayer].tileData->tileType == "water" ||
            m_mapNodeData[currentLayer].tileData->tileType == "terrain")
        {
          tileMap = TileMap::DEFAULT;
          m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
        }
        // if the node should autotile, check if it needs to tile itself to another tile of the same ID
        else if (m_mapNodeData[currentLayer].tileData->tileType == "autotile")
        {
          m_orientation = TileManager::instance().calculateTileOrientation(m_tileIDBitmask);
        }
      }
      else if (m_elevationOrientation >= TileSlopes::N && m_elevationOrientation <= TileSlopes::S)
      {
        if (m_mapNodeData[currentLayer].tileData->slopeTiles.fileName.empty())
        {
          tileMap = TileMap::DEFAULT;
          m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
        }
        else
        {
          tileMap = TileMap::SLOPES; // TileSlopes [N,E,w,S]
          m_orientation = m_elevationOrientation;
        }
      }
      else if (m_elevationOrientation >= TileSlopes::NW && m_elevationOrientation <= TileSlopes::S_AND_E)
      {
        if (m_mapNodeData[currentLayer].tileData->cornerTiles.fileName.empty())
        {
          tileMap = TileMap::DEFAULT;
          m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
        }
        else
        {
          tileMap = TileMap::CORNERS; // TileSlopes [NW,NE,SE,SW,N_AND_W,N_AND_E,S_AND_E,S_AND_W]
          m_orientation = m_elevationOrientation;
        }
      }

      switch (tileMap)
      {
      case TileMap::DEFAULT:
        if (!m_mapNodeData[currentLayer].shouldRender)
        {
          break;
        }
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->tiles.clippingWidth;
        if (m_mapNodeData[currentLayer].tileIndex != 0)
        {
          clipRect.x = m_clippingWidth * m_mapNodeData[currentLayer].tileIndex;
        }
        else
        {
          clipRect.x = m_clippingWidth * static_cast<int>(m_orientation);
        }
        if (!m_mapNodeData[currentLayer].tileID.empty())
        {

          m_sprite->setClipRect({clipRect.x + m_clippingWidth * m_mapNodeData[currentLayer].tileData->tiles.offset, 0,
                                 m_clippingWidth, m_mapNodeData[currentLayer].tileData->tiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID),
                               static_cast<Layer>(currentLayer));
        }

        spriteCount = m_mapNodeData[currentLayer].tileData->tiles.count;
        break;
      case TileMap::CORNERS:
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->cornerTiles.clippingWidth;
        clipRect.x = m_clippingWidth * (static_cast<int>(m_orientation) - 4);
        spriteCount = m_mapNodeData[currentLayer].tileData->cornerTiles.count;
        if (clipRect.x <= static_cast<int>(spriteCount) * m_clippingWidth)
        {
          m_sprite->setClipRect({clipRect.x + m_mapNodeData[currentLayer].tileData->cornerTiles.offset * m_clippingWidth, 0,
                                 m_clippingWidth, m_mapNodeData[currentLayer].tileData->cornerTiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID),
                               static_cast<Layer>(currentLayer));
        }
        break;
      case TileMap::SLOPES:
        if (m_mapNodeData[currentLayer].tileData->slopeTiles.fileName.empty())
        {
          break;
        }
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->slopeTiles.clippingWidth;
        clipRect.x = m_mapNodeData[currentLayer].tileData->slopeTiles.clippingWidth * static_cast<int>(m_orientation);
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

const MapNodeData &MapNode::getActiveMapNodeData() const
{
  //TODO: Needs further adjustments for other layers
  // Determine the topmost active layer here by checking if it has a tileID set and return it's mapNodeData
  if (MapLayers::isLayerActive(Layer::BUILDINGS) && !m_mapNodeData[Layer::BUILDINGS].tileID.empty() &&
      m_mapNodeData[Layer::BUILDINGS].tileData)
  {
    return m_mapNodeData[Layer::BUILDINGS];
  }

  return m_mapNodeData[Layer::TERRAIN];
}

void MapNode::setMapNodeData(std::vector<MapNodeData> &&mapNodeData)
{
  m_mapNodeData.swap(mapNodeData);

  // updates the pointers to the tiles, after loading tileIDs from json
  for (auto &it : m_mapNodeData)
  {
    if (it.tileData)
    {
      delete it.tileData;
    }
    it.tileData = TileManager::instance().getTileData(it.tileID);
  }
}

void MapNode::demolishNode()
{
  if (MapLayers::isLayerActive(Layer::BUILDINGS))
  {
    m_mapNodeData[Layer::BUILDINGS].tileData = nullptr;
    m_mapNodeData[Layer::BUILDINGS].tileID = "";
    m_sprite->clearSprite(Layer::BUILDINGS);
    this->m_origCornerPoint = this->getCoordinates();
    updateTexture();
  }
}
