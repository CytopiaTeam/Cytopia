#include "mapNode.hxx"

#include "../basics/log.hxx"
#include "../map/MapLayers.hxx"

MapNode::MapNode(Point isoCoordinates, const std::string &terrainID, const std::string &tileID)
    : m_isoCoordinates(std::move(isoCoordinates))
{
  mapNodeData.resize(LAYERS_COUNT);
  m_sprite = std::make_unique<Sprite>(m_isoCoordinates);

  setTileID(terrainID);
  if (!tileID.empty()) // in case tileID is not supplied skip it
    setTileID(tileID);

  updateTexture();
  MapLayers::enableLayer(Layer::TERRAIN);
  MapLayers::enableLayer(Layer::BUILDINGS);
}

void MapNode::increaseHeight()
{
  int height = m_isoCoordinates.height;

  if (height < m_maxHeight)
  {
    m_isoCoordinates.height++;
    m_sprite->isoCoordinates = m_isoCoordinates;
  }
}

void MapNode::decreaseHeight()
{
  int height = m_isoCoordinates.height;

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
    if (TileManager::instance().getTileData(tileID)->category == "Terrain")
    {
      m_previousTileID = mapNodeData[Layer::TERRAIN].tileID;
      mapNodeData[Layer::TERRAIN].tileData = TileManager::instance().getTileData(tileID);
      mapNodeData[Layer::TERRAIN].tileID = tileID;
    }
    else
    {
      m_previousTileID = mapNodeData[Layer::BUILDINGS].tileID;
      mapNodeData[Layer::BUILDINGS].tileData = TileManager::instance().getTileData(tileID);
      mapNodeData[Layer::BUILDINGS].tileID = tileID;
    }
    updateTexture();
  }
}

void MapNode::updateTexture()
{
  SDL_Rect clipRect{0, 0, 0, 0};

  tileMap = TileMap::DEFAULT;

  for (uint32_t it = 0; it < LAYERS_COUNT; ++it)
  {
    if (MapLayers::isLayerActive(it) && mapNodeData[it].tileData)
    {
      size_t spriteCount = 1;
      // only calculate orientation for textures that adjust themselves according to elevation / other tiles of the same id
      if (mapNodeData[it].tileData->category == "Terrain" || mapNodeData[it].tileData->category == "Roads" ||
          mapNodeData[it].tileData->category == "Water")
      {
        m_orientation = TileManager::instance().caluclateSlopeOrientation(m_elevationBitmask);

        if (m_orientation == TileSlopes::DEFAULT_ORIENTATION)
        {
          if (mapNodeData[it].tileData->category == "Water")
          {
            tileMap = TileMap::DEFAULT;
            m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
          }
          // if the node has no elevated neighbors, check if it needs to tile itself to another tile of the same ID
          if (mapNodeData[it].tileData->category != "Terrain")
          {
            m_orientation = TileManager::instance().caluclateTileOrientation(m_tileIDBitmask);
          }
        }
        else
        {
          tileMap = TileMap::SLOPES;
        }
      }
      else
      {
        m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
      }

      switch (tileMap)
      {
      case TileMap::DEFAULT:
        // if tileIndex is set, it means we take a single image out of a spritesheet
        if (mapNodeData[it].tileData->tileIndex != -1)
        {
          clipRect.x = mapNodeData[it].tileData->tiles.clippingWidth * mapNodeData[it].tileData->tileIndex;
        }
        else
        {
          clipRect.x = mapNodeData[it].tileData->tiles.clippingWidth * static_cast<int>(m_orientation);
        }
        m_clippingWidth = mapNodeData[it].tileData->tiles.clippingWidth;
        if (!mapNodeData[it].tileID.empty())
        {
          clipRect.x = 0;
          m_sprite->setClipRect(
              {0, 0, mapNodeData[it].tileData->tiles.clippingWidth, mapNodeData[it].tileData->tiles.clippingHeight},
              static_cast<Layer>(it));
          m_sprite->setTexture(TileManager::instance().getTexture(mapNodeData[it].tileID, tileMap), static_cast<Layer>(it));
        }

        spriteCount = mapNodeData[it].tileData->tiles.count;
        break;
      case TileMap::CORNERS:
        m_clippingWidth = mapNodeData[it].tileData->cornerTiles.clippingWidth;
        clipRect.x = mapNodeData[it].tileData->cornerTiles.clippingWidth * static_cast<int>(m_orientation);
        spriteCount = mapNodeData[it].tileData->cornerTiles.count;
        if (mapNodeData[it].tileData->category == "Terrain")
        {
          m_sprite->setClipRect({clipRect.x, 0, mapNodeData[it].tileData->cornerTiles.clippingWidth,
                                 mapNodeData[it].tileData->cornerTiles.clippingHeight},
                                static_cast<Layer>(it));
          m_sprite->setTexture(TileManager::instance().getTexture(mapNodeData[it].tileID, tileMap), static_cast<Layer>(it));
        }
        break;
      case TileMap::SLOPES:
        m_clippingWidth = mapNodeData[it].tileData->slopeTiles.clippingWidth;
        clipRect.x = mapNodeData[it].tileData->slopeTiles.clippingWidth * static_cast<int>(m_orientation);
        spriteCount = mapNodeData[it].tileData->slopeTiles.count;
        if (mapNodeData[it].tileData->category == "Terrain")
        {
          m_sprite->setClipRect({clipRect.x, 0, mapNodeData[it].tileData->slopeTiles.clippingWidth,
                                 mapNodeData[it].tileData->slopeTiles.clippingHeight},
                                static_cast<Layer>(it));
          m_sprite->setTexture(TileManager::instance().getTexture(mapNodeData[it].tileID, tileMap), static_cast<Layer>(it));
        }
        break;
      }
      if (clipRect.x >= static_cast<int>(spriteCount) * m_clippingWidth)
      {
        mapNodeData[it].tileID = m_previousTileID;
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

const mapNodeData &MapNode::getActiveMapNodeData() const
{
  //TODO: Needs further adjustments for other layers
  // Determine the topmost active layer here by checking if it has a tileID set and return it's mapnodeData
  if (MapLayers::isLayerActive(Layer::BUILDINGS) && !mapNodeData[Layer::BUILDINGS].tileID.empty())
  {
    return mapNodeData[Layer::BUILDINGS];
  }

  return mapNodeData[Layer::TERRAIN];
}
