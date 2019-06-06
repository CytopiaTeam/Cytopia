#include "MapNode.hxx"

#include "../basics/LOG.hxx"
#include "../map/MapLayers.hxx"

MapNode::MapNode(Point isoCoordinates, const std::string &terrainID, const std::string &tileID)
    : m_isoCoordinates(std::move(isoCoordinates))
{
  m_mapNodeData.resize(LAYERS_COUNT);
  m_sprite = std::make_unique<Sprite>(m_isoCoordinates);

  setTileID(terrainID);
  if (!tileID.empty()) // in case tileID is not supplied skip it
    setTileID(tileID);

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

void MapNode::setTileID(const std::string &tileID)
{
  if (TileManager::instance().getTileData(tileID))
  {
    if (TileManager::instance().getTileData(tileID)->category == "Terrain" ||
        TileManager::instance().getTileData(tileID)->category == "Water")
    {
      m_previousTileID = m_mapNodeData[Layer::TERRAIN].tileID;
      m_mapNodeData[Layer::TERRAIN].tileData = TileManager::instance().getTileData(tileID);
      m_mapNodeData[Layer::TERRAIN].tileID = tileID;
    }
    else
    {
      m_previousTileID = m_mapNodeData[Layer::BUILDINGS].tileID;
      m_mapNodeData[Layer::BUILDINGS].tileData = TileManager::instance().getTileData(tileID);
      m_mapNodeData[Layer::BUILDINGS].tileID = tileID;
    }
    updateTexture();
  }
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
      // only calculate orientation for textures that adjust themselves according to elevation / other tiles of the same id
      if (m_mapNodeData[currentLayer].tileData->category == "Terrain" ||
          m_mapNodeData[currentLayer].tileData->category == "Roads" || m_mapNodeData[currentLayer].tileData->category == "Water")
      {
        //m_orientation = TileManager::instance().calculateSlopeOrientation(m_elevationBitmask);

        if (m_elevationOrientation == TileSlopes::DEFAULT_ORIENTATION)
        {
          if (m_mapNodeData[currentLayer].tileData->category == "Water")
          {
            tileMap = TileMap::DEFAULT;
            m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
          }
          // if the node has no elevated neighbors, check if it needs to tile itself to another tile of the same ID
          if (m_mapNodeData[currentLayer].tileData->category != "Terrain")
          {
            m_orientation = TileManager::instance().calculateTileOrientation(m_tileIDBitmask);
          }
        }
        else
        {
          if (m_mapNodeData[currentLayer].tileData->slopeTiles.fileName.empty())
          {
            tileMap = TileMap::DEFAULT;
            m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
          }
          else
          {
            tileMap = TileMap::SLOPES;
            m_orientation = m_elevationOrientation;
          }
        }
      }
      else
      {
        m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
      }

      switch (tileMap)
      {
      case TileMap::DEFAULT:
        // if tileIndex is set, it means we take a single image out of a sprite sheet
        if (m_mapNodeData[currentLayer].tileData->tileIndex != -1)
        {
          clipRect.x =
              m_mapNodeData[currentLayer].tileData->tiles.clippingWidth * m_mapNodeData[currentLayer].tileData->tileIndex;
        }
        else
        {
          clipRect.x = m_mapNodeData[currentLayer].tileData->tiles.clippingWidth * static_cast<int>(m_orientation);
        }
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->tiles.clippingWidth;
        if (!m_mapNodeData[currentLayer].tileID.empty())
        {
          // terrain always uses the first tile
          if (m_mapNodeData[currentLayer].tileID == "terrain" || m_mapNodeData[currentLayer].tileID == "water")
          {
            clipRect.x = 0;
          }

          m_sprite->setClipRect({clipRect.x, 0, m_mapNodeData[currentLayer].tileData->tiles.clippingWidth,
                                 m_mapNodeData[currentLayer].tileData->tiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID, tileMap),
                               static_cast<Layer>(currentLayer));
        }

        spriteCount = m_mapNodeData[currentLayer].tileData->tiles.count;
        break;
      case TileMap::CORNERS:
        m_clippingWidth = m_mapNodeData[currentLayer].tileData->cornerTiles.clippingWidth;
        clipRect.x = m_mapNodeData[currentLayer].tileData->cornerTiles.clippingWidth * static_cast<int>(m_orientation);
        spriteCount = m_mapNodeData[currentLayer].tileData->cornerTiles.count;
        if (m_mapNodeData[currentLayer].tileData->category == "Terrain")
        {
          m_sprite->setClipRect({clipRect.x, 0, m_mapNodeData[currentLayer].tileData->cornerTiles.clippingWidth,
                                 m_mapNodeData[currentLayer].tileData->cornerTiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID, tileMap),
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
        if (clipRect.x < static_cast<int>(spriteCount) * m_clippingWidth)
        {
          m_sprite->setClipRect({clipRect.x, 0, m_mapNodeData[currentLayer].tileData->slopeTiles.clippingWidth,
                                 m_mapNodeData[currentLayer].tileData->slopeTiles.clippingHeight},
                                static_cast<Layer>(currentLayer));
          m_sprite->setTexture(TileManager::instance().getTexture(m_mapNodeData[currentLayer].tileID, tileMap),
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

void MapNode::demolishNode()
{
  if (MapLayers::isLayerActive(Layer::BUILDINGS))
  {
    m_mapNodeData[Layer::BUILDINGS].tileData = nullptr;
    m_mapNodeData[Layer::BUILDINGS].tileID = "";
    m_sprite->clearSprite(Layer::BUILDINGS);
    updateTexture();
  }
}
