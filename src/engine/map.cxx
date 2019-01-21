#include "map.hxx"

#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
#include "basics/resources.hxx"
#include "basics/log.hxx"
#include "textureManager.hxx"

constexpr struct
{
  int x;
  int y;

} adjecantNodesOffsets[9] = {
    {-1, -1}, // 6 = 2^6 = 64  = BOTTOM LEFT
    {-1, 0},  // 2 = 2^2 = 4   = LEFT
    {-1, 1},  // 4 = 2^4 = 16  = TOP LEFT
    {0, -1},  // 1 = 2^1 = 2   = BOTTOM
    {0, 0},   // center
    {0, 1},   // 0 = 2^0 = 1   = TOP
    {1, -1},  // 7 = 2^7 = 128 = BOTTOM RIGHT
    {1, 0},   // 3 = 2^3 = 8   = RIGHT
    {1, 1}    // 5 = 2^5 = 32  = TOP RIGHT
};

Map::Map(int columns, int rows) : _mapNodes((columns + 1) * (rows + 1)), _columns(columns + 1), _rows(rows + 1)
{
  _mapNodes.reserve(_rows * _columns);
  initMap();
}

void Map::initMap()
{
  int z = 0;

  // nodes need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x <= Settings::instance().settings.mapSize; x++)
  {
    for (int y = Settings::instance().settings.mapSize; y >= 0; y--)
    {
      _mapNodes[x * _columns + y] = new MapNode(Point{x, y, z++, 0});
      _mapNodesInDrawingOrder.push_back(_mapNodes[x * _columns + y]);
    }
  }
}

void Map::increaseHeight(const Point &isoCoordinates)
{
  int height = _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height < Settings::instance().settings.maxElevationHeight)
  {
    demolishNode(isoCoordinates);
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->increaseHeight();
    updateNeighbors(_mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void Map::decreaseHeight(const Point &isoCoordinates)
{
  int height = _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height > 0)
  {
    demolishNode(isoCoordinates);
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->decreaseHeight();
    updateNeighbors(_mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void Map::updateNeighbors(const Point &isoCoordinates)
{
  unsigned char elevationBitmask;
  int tileHeight = _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  NeighborMatrix matrix;
  getNeighbors(isoCoordinates, matrix);

  int i = 0;
  for (const auto &it : matrix)
  {
    if (it)
    {
      bool raise = false;
      elevationBitmask = getElevatedNeighborBitmask(it->getCoordinates());

      // if the elevation bitmask changes (-> a new texture is drawn), demolish the tile
      if (elevationBitmask != it->getElevationBitmask())
      {
        demolishNode(it->getCoordinates());
      }

      // set elevation and tile bitmask for each neighbor
      it->setBitmask(elevationBitmask, getNeighboringTilesBitmask(it->getCoordinates()));

      // there can't be a height difference greater then 1 between two map nodes.
      // only increase the cardinal directions
      if (i % 2)
      {
        if (tileHeight - it->getCoordinates().height > 1)
        {

          increaseHeight(it->getCoordinates());
        }
        else if (tileHeight - it->getCoordinates().height < -1)
        {
          decreaseHeight(it->getCoordinates());
        }
      }

      // those bitmask combinations require the tile to be elevated.
      std::vector<unsigned char> bits{3, 12, 26, 38, 73, 133};
      for (auto elevationBit : bits)
      {
        if ((elevationBitmask & elevationBit) == elevationBit)
        {
          raise = true;
        }
      }
      if (raise)
      {

        increaseHeight(it->getCoordinates());
        if (terrainEditMode == TerrainEdit::LOWER)
        {
          //decreaseHeight(it->getCoordinates());
          NeighborMatrix loweredNodesNeighbors;
          getNeighbors(it->getCoordinates(), loweredNodesNeighbors);
          for (const auto &it : loweredNodesNeighbors)
          {
            if (it)
            {
              if (it->getCoordinates().height > tileHeight)
              {
                decreaseHeight(it->getCoordinates());
              }
            }
          }
        }
      }
    }
    i++;
  }
}

void Map::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID)
{
  _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->setTileID(tileID);
  updateNeighbors(isoCoordinates);
}

unsigned char Map::getElevatedNeighborBitmask(const Point &isoCoordinates)
{
  unsigned char bitmask = 0;
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;

  std::pair<int, int> adjecantNodesCoordinates[8] = {
      std::make_pair(x, y + 1),     // 0 = 2^0 = 1   = TOP
      std::make_pair(x, y - 1),     // 1 = 2^1 = 2   = BOTTOM
      std::make_pair(x - 1, y),     // 2 = 2^2 = 4   = LEFT
      std::make_pair(x + 1, y),     // 3 = 2^3 = 8   = RIGHT
      std::make_pair(x - 1, y + 1), // 4 = 2^4 = 16  = TOP LEFT
      std::make_pair(x + 1, y + 1), // 5 = 2^5 = 32  = TOP RIGHT
      std::make_pair(x - 1, y - 1), // 6 = 2^6 = 64  = BOTTOM LEFT
      std::make_pair(x + 1, y - 1)  // 7 = 2^7 = 128 = BOTTOM RIGHT
  };

  int i = 0;
  for (const auto &it : adjecantNodesCoordinates)
  {
    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {
      if (_mapNodes[it.first * _columns + it.second]->getCoordinates().height >
              _mapNodes[x * _columns + y]->getCoordinates().height &&
          _mapNodes[it.first * _columns + it.second])
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(1 << i);
      }
    }
    i++;
  }
  return bitmask;
}

unsigned char Map::getNeighboringTilesBitmask(const Point &isoCoordinates)
{
  unsigned char bitmask = 0;
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;

  if (_mapNodes[x * _columns + y]->getTileData()->type == "terrain")
  {
    return bitmask;
  }

  std::pair<int, int> adjecantNodesCoordinates[8] = {
      std::make_pair(x, y + 1),     // 0 = 2^0 = 1   = TOP
      std::make_pair(x, y - 1),     // 1 = 2^1 = 2   = BOTTOM
      std::make_pair(x - 1, y),     // 2 = 2^2 = 4   = LEFT
      std::make_pair(x + 1, y),     // 3 = 2^3 = 8   = RIGHT
      std::make_pair(x - 1, y + 1), // 4 = 2^4 = 16  = TOP LEFT
      std::make_pair(x + 1, y + 1), // 5 = 2^5 = 32  = TOP RIGHT
      std::make_pair(x - 1, y - 1), // 6 = 2^6 = 64  = BOTTOM LEFT
      std::make_pair(x + 1, y - 1)  // 7 = 2^7 = 128 = BOTTOM RIGHT
  };

  int i = 0;
  for (const auto &it : adjecantNodesCoordinates)
  {
    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {
      if (_mapNodes[it.first * _columns + it.second]->getTileID() == _mapNodes[x * _columns + y]->getTileID())
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(1 << i);
      }
    }
    i++;
  }
  return bitmask;
}

void Map::getNeighbors(const Point &isoCoordinates, NeighborMatrix &result) const
{
  size_t idx = 0;
  for (const auto &it : adjecantNodesOffsets)
  {
    int x = isoCoordinates.x + it.x;
    int y = isoCoordinates.y + it.y;
    if (x >= 0 && x < _rows && y >= 0 && y < _columns)
    {
      result[idx] = _mapNodes[x * _columns + y];
    }
    else
    {
      result[idx] = nullptr;
    }
    ++idx;
  }
}
void Map::renderMap()
{
  for (auto it : _mapNodesInDrawingOrder)
  {
    it->render();
  }
}

void Map::refresh()
{
  for (auto it : _mapNodesInDrawingOrder)
  {
    it->getSprite()->refresh();
  }
}

SDL_Color Map::getColorOfPixelInSurface(SDL_Surface *surface, int x, int y, const SDL_Rect &clipRect) const
{
  SDL_Color Color = {0, 0, 0, SDL_ALPHA_TRANSPARENT};

  x += clipRect.w;

  if (surface)
  {
    int Bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * Bpp;
    Uint32 pixel = *(Uint32 *)p;

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }

  return Color;
}

Point Map::findNodeInMap(const Point &screenCoordinates) const
{
  Point foundCoordinates{-1, -1, 0, 0};

  // calculate clicked column (x coordinate) without heigh taken into account.
  int isoX = static_cast<int>(
      (screenCoordinates.x + Resources::getCameraOffset().x + 2.0 * (screenCoordinates.y + Resources::getCameraOffset().y)) /
          (32 * Resources::getZoomLevel()) +
      1);
  int isoY = static_cast<int>(
      (screenCoordinates.x + Resources::getCameraOffset().x - 2.0 * (screenCoordinates.y + Resources::getCameraOffset().y)) /
      (32 * Resources::getZoomLevel()));

  bool run = true;
  // traverse a column from top to bottom (from the calculated coordinates)
  while (isoX < Settings::instance().settings.mapSize && isoY > 0)
  {
    if (isClickWithinTile(screenCoordinates, isoX, isoY))
    {
      if (foundCoordinates.z < _mapNodes[isoX * _columns + isoY]->getCoordinates().z)
      {
        foundCoordinates = _mapNodes[isoX * _columns + isoY]->getCoordinates();
      }
    }
    if (isClickWithinTile(screenCoordinates, isoX - 1, isoY))
    {
      if (foundCoordinates.z < _mapNodes[(isoX - 1) * _columns + isoY]->getCoordinates().z)
      {
        foundCoordinates = _mapNodes[(isoX - 1) * _columns + isoY]->getCoordinates();
      }
    }
    if (isClickWithinTile(screenCoordinates, isoX, isoY + 1))
    {
      if (foundCoordinates.z < _mapNodes[isoX * _columns + (isoY + 1)]->getCoordinates().z)
      {
        foundCoordinates = _mapNodes[isoX * _columns + (isoY + 1)]->getCoordinates();
      }
    }

    isoX++;
    isoY--;
  }

  return foundCoordinates;
}

void Map::demolishNode(const Point &isoCoordinates)
{
  _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->setTileID("terrain");
}

bool Map::isClickWithinTile(const Point &screenCoordinates, int isoX, int isoY) const
{

  if (isoX < 0 || isoX > Settings::instance().settings.mapSize || isoY < 0 || isoY > Settings::instance().settings.mapSize)
  {
    return false;
  }

  Point foundCoordinates{-1, -1, 0, 0};
  SDL_Rect spriteRect = _mapNodes[isoX * _columns + isoY]->getSprite()->getDestRect();
  SDL_Point clicked = {screenCoordinates.x, screenCoordinates.y};
  if (SDL_PointInRect(&clicked, &spriteRect))
  {
    // Calculate the position of the clicked pixel within the surface and "un-zoom" the positon to match the un-adjusted surface
    int pixelX = static_cast<int>((screenCoordinates.x - spriteRect.x) / Resources::getZoomLevel());
    int pixelY = static_cast<int>((screenCoordinates.y - spriteRect.y) / Resources::getZoomLevel());

    // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
    if (getColorOfPixelInSurface(TextureManager::instance().getTileSurface(_mapNodes[isoX * _columns + isoY]->getTileID(),
                                                                           _mapNodes[isoX * _columns + isoY]->getUsedTileMap()),
                                 pixelX, pixelY, _mapNodes[isoX * _columns + isoY]->getSprite()->getClipRect())
            .a != SDL_ALPHA_TRANSPARENT)
    {
      return true;
    }
  }
  return false;
}

void Map::highlightNode(const Point &isoCoordinates)
{
  if (highlitNode != nullptr)
  {
    highlitNode->getSprite()->highlight(false);
  }

  int index = isoCoordinates.x * _columns + isoCoordinates.y;

  if (index >= 0 && index < _mapNodes.size())
  {
    highlitNode = _mapNodes.at(index);
    highlitNode->getSprite()->highlight(true);
  }
}
