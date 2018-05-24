#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix()
{

}

vectorMatrix::vectorMatrix(int columns, int rows)
  : _cellMatrix((columns+1) * (rows+1)), _columns(columns+1), _rows(rows+1)
{
  _cellMatrix.reserve(_rows * _columns);
} 

vectorMatrix::~vectorMatrix()
{

}

void vectorMatrix::resizeMatrix(int rows, int columns)
{

  _cellMatrix.resize(_rows);

  //for (auto &it : _cellMatrix)
  //{
  //  it.resize(_columns);
  //}
}

  void vectorMatrix::addCell(int x, int y,int z)
  {
    std::shared_ptr<Cell> cellPtr(new Cell(Point(x, y, z)));
    _cellMatrix[x * _columns + y]= (cellPtr);
  }

  std::shared_ptr<Cell> vectorMatrix::getCell(int x, int y)
  {
    return _cellMatrix[x * _columns + y];
  }

  std::vector<std::shared_ptr<Cell>> vectorMatrix::getCellNeighbors(int x, int y)
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
    for (int x = 0; x < _rows; x++)
    {
      for (int y = 0; y < _columns; y++)
      {
        _cellMatrix[x * _columns + y]->setNeighbors(getCellNeighbors(x, y));
      }
    }
  }
