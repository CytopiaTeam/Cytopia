#include "vectorMatrix.hxx"

constexpr struct
{
  int x;
  int y;

} adjecantCellOffsets[9] = {
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

vectorMatrix::vectorMatrix(int columns, int rows)
    : _cellMatrix((columns + 1) * (rows + 1)), _columns(columns + 1), _rows(rows + 1)
{
  _cellMatrix.reserve(_rows * _columns);
  initCells();
}

void vectorMatrix::initCells()
{
  int z = 0;

  // cells need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x <= Settings::Instance().settings.mapSize; x++)
  {
    for (int y = Settings::Instance().settings.mapSize; y >= 0; y--)
    {
      z++;
      //_cellMatrix.emplace(_cellMatrix.begin() + x * _columns + y,std::make_unique<Cell>(Point(x, y, z)));
      _cellMatrix[x * _columns + y] = std::make_shared<Cell>(Point{x, y, z, 0});
    }
  }
}

void vectorMatrix::increaseHeightOfCell(const Point &isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height < Settings::Instance().settings.maxElevationHeight)
  {
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->increaseHeightOfCell();
    drawSurroundingTiles(_cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
  }
}

void vectorMatrix::decreaseHeightOfCell(const Point &isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height > 0)
  {
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->decreaseHeightOfCell();
    drawSurroundingTiles(_cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
  }
}

void vectorMatrix::drawSurroundingTiles(const Point &isoCoordinates)
{
  int tileHeight = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;
  int i = 0;
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;
  int currentX, currentY;

  for (auto it : adjecantCellOffsets)
  {
    currentX = x + it.x;
    currentY = y + it.y;

    if (currentX >= 0 && currentX < _rows && currentY >= 0 && currentY < _columns)
    {
      if (_cellMatrix[currentX * _columns + currentY])
      {
        Point currentCoords = _cellMatrix[currentX * _columns + currentY]->getCoordinates();

        // there can't be a height difference greater then 1 between two map cells.
        if (tileHeight - _cellMatrix[currentX * _columns + currentY]->getCoordinates().height > 1 &&
            Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE && i % 2)
        {
          increaseHeightOfCell(currentCoords);
        }
        else if (tileHeight - _cellMatrix[currentX * _columns + currentY]->getCoordinates().height < -1 &&
                 Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER && i % 2)
        {
          decreaseHeightOfCell(currentCoords);
        }
        determineTileIDOfCell(_cellMatrix[currentX * _columns + currentY]->getCoordinates());
      }
    }
    i++;
  }
}

void vectorMatrix::determineTileIDOfCell(const Point &isoCoordinates)
{
  unsigned int _elevatedTilePosition = 0;
  int tileHeight = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;
  int newTileID = -2; // set to -2 to determine if it's necessary to set a new tile ID

  int x = isoCoordinates.x;
  int y = isoCoordinates.y;
  int currentX, currentY;

  _elevatedTilePosition = getElevatedNeighborBitmask(isoCoordinates);
  auto keyTileID = Resources::slopeTileIDMap.find(_elevatedTilePosition);

  if (keyTileID != Resources::slopeTileIDMap.end())
  {
    newTileID = keyTileID->second;
  }
  // special case: if both opposite neighbors are elevated, the center tile also gets elevated
  constexpr auto LEFT_and_RIGHT = ELEVATED_LEFT | ELEVATED_RIGHT;
  constexpr auto TOP_and_BOTTOM = ELEVATED_TOP | ELEVATED_BOTTOM;

  // LEFT-RIGHT / TOP-BOTTOM combinations are handled here because there are too many possible combinations
  // check if it could help to also handle the other diagobal combinations here too.
  if ((_elevatedTilePosition & LEFT_and_RIGHT) == LEFT_and_RIGHT || (_elevatedTilePosition & TOP_and_BOTTOM) == TOP_and_BOTTOM ||
      newTileID == -1)
  {
    if (Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE)
    {
      increaseHeightOfCell(_cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    }
    else if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
    {
      for (auto it : adjecantCellOffsets)
      {
        currentX = x + it.x;
        currentY = y + it.y;

        if (_cellMatrix[currentX * _columns + currentY]->getCoordinates().height > tileHeight &&
            _cellMatrix[currentX * _columns + currentY])
        {
          decreaseHeightOfCell(_cellMatrix[currentX * _columns + currentY]->getCoordinates());
        }
      }
    }
    // Tile gets elevated, so tile id must be 14
    newTileID = 14;
  }

  if (newTileID != -2)
  {
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->setTileID(newTileID);
  }
  else
  {
    LOG(LOG_ERROR) << "It seems there is no combination for bitmask: " << _elevatedTilePosition;
  }
}

unsigned int vectorMatrix::getElevatedNeighborBitmask(const Point &isoCoordinates)
{
  unsigned int bitmask = 0;
  int x = isoCoordinates.x;
  int y = isoCoordinates.y;

  std::pair<int, int> adjecantCellCoordinates[8] = {
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
  for (auto it : adjecantCellCoordinates)
  {
    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {
      if (_cellMatrix[it.first * _columns + it.second]->getCoordinates().height >
              _cellMatrix[x * _columns + y]->getCoordinates().height &&
          _cellMatrix[it.first * _columns + it.second])
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(std::pow(2, i));
      }
    }
    i++;
  }
  return bitmask;
}

void vectorMatrix::getNeighbors(const Point &isoCoordinates, NeighborMatrix &result) const
{
  std::vector<std::shared_ptr<Cell>> neighborCells;
  neighborCells.reserve(9);

  size_t idx = 0;
  for (const auto &it : adjecantCellOffsets)
  {
    int x = isoCoordinates.x + it.x;
    int y = isoCoordinates.y + it.y;
    if (x >= 0 && x < _rows && y >= 0 && y < _columns)
    {
      result[idx] = &*_cellMatrix[x * _columns + y];
    }
    else
    {
      result[idx] = nullptr;
    }
    ++idx;
  }
}