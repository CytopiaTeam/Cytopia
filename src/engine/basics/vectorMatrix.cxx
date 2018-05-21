#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix()
{

}

vectorMatrix::vectorMatrix(int columns, int rows)
{
  _matrix.reserve((rows + 1) * (columns + 1));
  //_matrix.resize((rows + 1) * (columns + 1));
  _cellMatrix.reserve((rows + 1) * (columns + 1));
  //resizeMatrix(columns, rows);
  _columns = columns + 1;
  _rows = rows + 1;
} 

vectorMatrix::~vectorMatrix()
{

}


//void vectorMatrix::resizeMatrix(int rows, int columns)
//{
//  _columns = columns + 1;
//  _rows = rows + 1;
//
//  _matrix.resize(_rows);
//  _cellMatrix.resize(_rows);
//
//  for (auto &it : _matrix)
//  {
//    it.resize(_columns);
//  }
//
//  for (auto &it : _cellMatrix)
//  {
//    it.resize(_columns);
//  }
//}

void vectorMatrix::addSprite(int x, int y, Sprite* sprite)
{
  //return mData[i * mCols + j];
  _matrix[x * _columns + y] = sprite;
  _matrix[x * _columns + y] = sprite;
  //_matrix[x][y] = sprite;
}

Sprite* vectorMatrix::getSprite(int x, int y)
{
  return _matrix[x * _columns + y];
}

void vectorMatrix::removeSprite(int x, int y)
{
  _matrix[x * _columns + y] = nullptr;
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
        if (_matrix[currentRow * _columns + currentColumn] != nullptr )
          neighbors.push_back(_matrix[currentRow * _columns + currentColumn]);
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
    _cellMatrix[x * _columns + y] = cell;
  }

  Cell* vectorMatrix::getCell(int x, int y)
  {
    return _cellMatrix[x * _columns + y];
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
    int z = 0;
    // initialize cell Matrix
    for (int x = 0; x <= Resources::settings.mapSize; x++)
    {
      for (int y = Resources::settings.mapSize; y >= 0; y--)
      {
        z++;
        Cell* mapCell = new Cell(Point(x, y, z));
        //_floorCellMatrix.addCell(x, y, mapCell);
        _cellMatrix.push_back(mapCell);
      }
    }


    for (int x = 0; x < _rows; x++)
    {
      for (int y = 0; y < _columns; y++)
      {
        _cellMatrix[x * _columns + y]->setNeighbors(getCellNeighbors(x, y));
      }
    }
  }