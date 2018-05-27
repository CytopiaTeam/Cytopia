#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix(int columns, int rows)
  : _cellMatrix((columns+1) * (rows+1)), _columns(columns+1), _rows(rows+1)
{
  _cellMatrix.reserve(_rows * _columns);
  initCells();
} 

std::vector<std::shared_ptr<Cell>> vectorMatrix::getCellNeighbors(int x, int y)
{
  std::vector<std::shared_ptr<Cell>> neighbors;
  neighbors.reserve(9);

  for (int rowIterator = -1; rowIterator <= 1; rowIterator++)
  {
    int currentRow = x + rowIterator;

    for (int columnIterator = -1; columnIterator <= 1; columnIterator++)
    {
      int currentColumn = y + columnIterator;

      // check if the neighbor is within bounds of the tilemap
      if ( currentRow >= 0 && currentRow < _rows && currentColumn >= 0 && currentColumn < _columns && !(currentRow == x && currentColumn == y) )
      {
        if ( _cellMatrix[currentRow * _columns + currentColumn] )
          neighbors.push_back(_cellMatrix[currentRow * _columns + currentColumn]);
      }
      else
      {
        neighbors.push_back(nullptr);
      }
    }
  }
  return neighbors;
}

void vectorMatrix::initCells()
{

  // initialize cell Matrix
  int z = 0;

  for (int x = 0; x <= Resources::settings.mapSize; x++)
  {
    for (int y = Resources::settings.mapSize; y >= 0; y--)
    {
      // cells need to be created at the correct vector "coordinates", or else the Z-Order will be broken
      z++;
      _cellMatrix[x * _columns + y] = std::shared_ptr<Cell>(new Cell(Point(x, y, z)));
    }
  }

  // neighbors must be set after all the cells are there
  for (auto &it : _cellMatrix)
  {
    it->setNeighbors(getCellNeighbors(it->getCoordinates().getX(), it->getCoordinates().getY()));
  }
}