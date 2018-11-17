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
  for (int x = 0; x <= Settings::Instance().settings.mapSize; x++)
  {
    for (int y = Settings::Instance().settings.mapSize; y >= 0; y--)
    {
      _mapNodes[x * _columns + y] = new MapNode(Point{x, y, z++, 0});
      _mapNodesInDrawingOrder.push_back(_mapNodes[x * _columns + y]);
    }
  }
}

void Map::increaseHeight(const Point &isoCoordinates)
{
  int height = _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height < Settings::Instance().settings.maxElevationHeight)
  {
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->increaseHeight();
    updateNeightbors(_mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void Map::decreaseHeight(const Point &isoCoordinates)
{
  int height = _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height > 0)
  {
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->decreaseHeight();
    updateNeightbors(_mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void Map::updateNeightbors(const Point &isoCoordinates)
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
      // set elevation bitmask for each neighbor
      it->setElevationBitmask(elevationBitmask);

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
      for (auto it : bits)
      {
        if ((elevationBitmask & it) == it)
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

void Map::setTileTypeOfNode(const Point &isoCoordinates, const std::string &tileType)
{
  _mapNodes[isoCoordinates.x * _columns + isoCoordinates.y]->setTileType(tileType);
}

unsigned int Map::getElevatedNeighborBitmask(const Point &isoCoordinates)
{
  unsigned int bitmask = 0;
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

Point Map::findNodeInMap(const Point &screenCoordinates, float zoomLevel) const
{
  Point foundCoordinates{-1, -1, 0, 0};

  // check all nodes of the map to find the clicked point
  for (auto it : _mapNodesInDrawingOrder)
  {
    //MapNode *it = _mapNodes[x * _columns * y];

    SDL_Rect spriteRect = it->getSprite()->getDestRect();

    int clickedX = screenCoordinates.x;
    int clickedY = screenCoordinates.y;

    int spriteX = spriteRect.x;
    int spriteY = spriteRect.y;

    if (clickedX >= spriteX && clickedX < spriteX + spriteRect.w && clickedY >= spriteY && clickedY < spriteY + spriteRect.h)
    {
      // Calculate the position of the clicked pixel within the surface
      int pixelX = (clickedX - spriteX);
      int pixelY = (clickedY - spriteY);
      // "un-zoom" the positon to match the un-adjusted surface
      pixelX = static_cast<int>(pixelX / zoomLevel);
      pixelY = static_cast<int>(pixelY / zoomLevel);

      // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
      if (getColorOfPixelInSurface(TextureManager::Instance().getTileSurface(it->getTileType(), it->getUsedTileMap()), pixelX,
                                   pixelY, it->getSprite()->getClipRect())
              .a != SDL_ALPHA_TRANSPARENT)
      {
        if (foundCoordinates.z < it->getCoordinates().z)
        {
          foundCoordinates = it->getCoordinates();
        }
      }
    }
  }
  return foundCoordinates;
}