#include "vectorMatrix.hxx"

#include "settings.hxx"
#include "resources.hxx"
#include "log.hxx"

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
      _cellMatrix[x * _columns + y] = new Cell(Point{x, y, z++, 0});
      _cellMatrixDrawingOrder.push_back(_cellMatrix[x * _columns + y]);
    }
  }
}

void vectorMatrix::increaseHeightOfCell(const Point &isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height < Settings::Instance().settings.maxElevationHeight)
  {
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->increaseHeightOfCell();
    updateNeightbors(_cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    // TODO call refresh properly
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void vectorMatrix::decreaseHeightOfCell(const Point &isoCoordinates)
{
  int height = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

  if (height > 0)
  {
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->decreaseHeightOfCell();
    updateNeightbors(_cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates());
    _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getSprite()->refresh();
  }
}

void vectorMatrix::updateNeightbors(const Point &isoCoordinates)
{
  unsigned char elevationBitmask;
  int tileHeight = _cellMatrix[isoCoordinates.x * _columns + isoCoordinates.y]->getCoordinates().height;

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

      // there can't be a height difference greater then 1 between two map cells.
      // only increase the cardinal directions
      if (i % 2)
      {
        if (tileHeight - it->getCoordinates().height > 1)
        {
          increaseHeightOfCell(it->getCoordinates());
        }
        else if (tileHeight - it->getCoordinates().height < -1)
        {
          decreaseHeightOfCell(it->getCoordinates());
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
        increaseHeightOfCell(it->getCoordinates());

        if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
        {
          //decreaseHeightOfCell(it->getCoordinates());
          NeighborMatrix loweredCellNeighbors;
          getNeighbors(it->getCoordinates(), loweredCellNeighbors);
          for (const auto &it : loweredCellNeighbors)
          {
            if (it)
            {
              if (it->getCoordinates().height > tileHeight)
              {
                decreaseHeightOfCell(it->getCoordinates());
              }
            }
          }
        }
      }
    }
    i++;
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
  for (const auto &it : adjecantCellCoordinates)
  {
    if (it.first >= 0 && it.first < _rows && it.second >= 0 && it.second < _columns)
    {
      if (_cellMatrix[it.first * _columns + it.second]->getCoordinates().height >
              _cellMatrix[x * _columns + y]->getCoordinates().height &&
          _cellMatrix[it.first * _columns + it.second])
      {
        // for each found tile add 2 ^ i to the bitmask
        bitmask |= static_cast<unsigned int>(1 << i);
      }
    }
    i++;
  }
  return bitmask;
}

void vectorMatrix::getNeighbors(const Point &isoCoordinates, NeighborMatrix &result) const
{
  size_t idx = 0;
  for (const auto &it : adjecantCellOffsets)
  {
    int x = isoCoordinates.x + it.x;
    int y = isoCoordinates.y + it.y;
    if (x >= 0 && x < _rows && y >= 0 && y < _columns)
    {
      result[idx] = _cellMatrix[x * _columns + y];
    }
    else
    {
      result[idx] = nullptr;
    }
    ++idx;
  }
}
void vectorMatrix::renderMatrix()
{
  for (auto it : _cellMatrixDrawingOrder)
  {
    it->render();
  }
}

void vectorMatrix::refresh()
{
  for (auto it : _cellMatrixDrawingOrder)
  {
    it->getSprite()->refresh();
  }
}
