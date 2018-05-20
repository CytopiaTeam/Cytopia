#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix()
{

}

vectorMatrix::vectorMatrix(int columns, int rows)
  : _matrix(rows+1, std::vector<Sprite*>(columns+1))
{
  resizeMatrix(columns, rows);
  _columns = columns + 1;
  _rows = rows + 1;
} 

vectorMatrix::~vectorMatrix()
{

}


void vectorMatrix::resizeMatrix(int rows, int columns)
{
  _columns = columns + 1;
  _rows = rows + 1;

  _matrix.resize(_rows);
  _cellMatrix.resize(_rows);

  for (auto &it : _matrix)
  {
    it.resize(_columns);
  }

  for (auto &it : _cellMatrix)
  {
    it.resize(_columns);
  }
}

void vectorMatrix::addSprite(int x, int y, Sprite* sprite)
{
  _matrix[x][y] = sprite;
}

Sprite* vectorMatrix::getSprite(int x, int y)
{
  return _matrix[x][y];
}

void vectorMatrix::removeSprite(int x, int y)
{
  _matrix[x][y] = nullptr;
}

void vectorMatrix::clearMatrix()
{
  // CLear it!
  for (int x = 0; x < _rows; x++)
  {
    for (int y = 0; y < _columns; y++)
    {
      removeSprite(x, y);
    }
  }
}

std::vector<Sprite*> vectorMatrix::findNeighbors(int x, int y)
{
  std::vector<Sprite*> neighbors;

  for (int rowIterator = -1; rowIterator <= 1; rowIterator++)
  {
    int currentRow = x + rowIterator;

    for (int columnIterator = -1; columnIterator <= 1; columnIterator++)
    {
      int currentColumn = y + columnIterator;

      // check if the neighbor is within bounds of the tilemap
      if ( currentRow >= 0 && currentRow < _rows && currentColumn >= 0 && currentColumn < _columns && !(currentRow == x && currentColumn == y) )
      {
        if ( _matrix[currentRow][currentColumn] != nullptr )
          neighbors.push_back(_matrix[currentRow][currentColumn]);
      }
      else
      {
        neighbors.push_back(nullptr);
      }
    }
  }
  return neighbors;
}

  void vectorMatrix::addCell(int x, int y, Cell* cell)
  {
    _cellMatrix[x][y] = cell;
  }

  Cell* vectorMatrix::getCell(int x, int y)
  {
    return _cellMatrix[x][y];
  }

  std::vector<Cell*> vectorMatrix::getCellNeighbors(int x, int y)
  {
    std::vector<Cell*> neighbors;
    neighbors.reserve(8);

    for (int rowIterator = -1; rowIterator <= 1; rowIterator++)
    {
      int currentRow = x + rowIterator;

      for (int columnIterator = -1; columnIterator <= 1; columnIterator++)
      {
        int currentColumn = y + columnIterator;

        // check if the neighbor is within bounds of the tilemap
        if ( currentRow >= 0 && currentRow < _rows && currentColumn >= 0 && currentColumn < _columns && !(currentRow == x && currentColumn == y) )
        {
          if ( _cellMatrix[currentRow][currentColumn] != nullptr )
            neighbors.push_back(_cellMatrix[currentRow][currentColumn]);
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
        _cellMatrix[x][y]->setNeighbors(getCellNeighbors(x, y));
      }
    }
  }