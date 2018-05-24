#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix(int columns, int rows)
  : _cellMatrix((columns+1) * (rows+1)), _columns(columns+1), _rows(rows+1)
{
  _cellMatrix.reserve(_rows * _columns);
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
        if ( _cellMatrix[currentRow * _columns + currentColumn] != nullptr )
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
  for (int x = 0; x < _rows; x++)
  {
    for (int y = 0; y < _columns; y++)
    {
      _cellMatrix[x * _columns + y]->setNeighbors(getCellNeighbors(x, y));
    }
  }
}
