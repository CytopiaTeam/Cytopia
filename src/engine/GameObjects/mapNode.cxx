#include "mapNode.hxx"
#include "../basics/LOG.hxx"

MapNode::MapNode(Point isoCoordinates) : m_isoCoordinates(std::move(isoCoordinates))
{
  m_sprite = std::make_unique<Sprite>(m_isoCoordinates);
  updateTexture();
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
  m_previousTileID = m_tileID;
  m_tileID = tileID;
  updateTexture();
}

void MapNode::updateTexture()
{
  SDL_Rect clipRect{0, 0, 0, 0};

  m_tileData = TileManager::instance().getTileData(m_tileID);
  tileMap = TileMap::DEFAULT;

  if (m_tileData)
  {
    size_t spriteCount = 1;

    // only calculate orientation for textures that adjust themselves according to elevation / other tiles of the same id
    if (m_tileData->category == "Terrain" || m_tileData->category == "Roads" || m_tileData->category == "Water")
    {
      m_orientation = TileManager::instance().caluclateSlopeOrientation(m_elevationBitmask);

      // if the node has no elevated neighbors, check if it needs to tile itself to another tile of the same ID
      if (m_orientation == TileSlopes::DEFAULT_ORIENTATION)
      {
        if (m_tileData->category == "Water")
        {
          tileMap = TileMap::DEFAULT;
          m_orientation = TileList::TILE_DEFAULT_ORIENTATION;
        }
        else if (m_tileData->category != "Terrain")
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
      if (m_tileData->tileIndex != -1)
      {
        clipRect.x = m_tileData->tiles.clippingWidth * m_tileData->tileIndex;
      }
      else
      {
        clipRect.x = m_tileData->tiles.clippingWidth * static_cast<int>(m_orientation);
      }
      m_clippingWidth = m_tileData->tiles.clippingWidth;
      if (m_tileID == "terrain")
      {
        clipRect.x = 0;
        m_sprite->clipRect = {0, 0, m_tileData->tiles.clippingWidth, m_tileData->tiles.clippingHeight};
      }
      else
      {
        m_sprite->clipRect = {clipRect.x, 0, m_tileData->tiles.clippingWidth, m_tileData->tiles.clippingHeight};
      }
      spriteCount = m_tileData->tiles.count;
      break;
    case TileMap::CORNERS:
      m_clippingWidth = m_tileData->cornerTiles.clippingWidth;
      clipRect.x = m_tileData->cornerTiles.clippingWidth * static_cast<int>(m_orientation);
      m_sprite->clipRect = {clipRect.x, 0, m_tileData->cornerTiles.clippingWidth, m_tileData->cornerTiles.clippingHeight};
      spriteCount = m_tileData->cornerTiles.count;
      break;
    case TileMap::SLOPES:
      m_clippingWidth = m_tileData->slopeTiles.clippingWidth;
      clipRect.x = m_tileData->slopeTiles.clippingWidth * static_cast<int>(m_orientation);
      m_sprite->clipRect = {clipRect.x, 0, m_tileData->slopeTiles.clippingWidth, m_tileData->slopeTiles.clippingHeight};
      spriteCount = m_tileData->slopeTiles.count;
      break;
    }
    if (clipRect.x >= static_cast<int>(spriteCount) * m_clippingWidth)
    {
      m_tileID = m_previousTileID;
      updateTexture();
    }
    m_sprite->spriteCount = spriteCount;
    m_sprite->setTexture(TileManager::instance().getTexture(m_tileID, tileMap));
  }
  else
  {
    LOG(LOG_ERROR) << "Could not retrieve TileData Information for TileID " << m_tileID;
  }
}

void MapNode::setCoordinates(const Point &newIsoCoordinates)
{
  m_isoCoordinates = newIsoCoordinates;
  m_sprite->isoCoordinates = m_isoCoordinates;
}
