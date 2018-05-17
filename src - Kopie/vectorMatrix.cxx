//#include "vectorMatrix.hxx"
//
//vectorMatrix::vectorMatrix(int columns, int rows)
//{
//  this->columns = columns;
//  this->rows = rows;
//
//  std::vector<Sprite>* cell = new std::vector<Sprite>*[columns];
//
//  for (int i = 0; i < columns; i++)
//  {
//    cell[i] = new std::vector<Sprite>[rows];
//  }
//}
//
//void vectorMatrix::addCellAt(int row, int column, const Sprite& entry)
//{
//  cell[channel][timeSlot].push_back(entry);
//}
//
//std::vector<Sprite>* vectorMatrix::getCell(int row, int column)
//{
//  return &cell[row][column];
//}
//
//void vectorMatrix::clearMatrix()
//{
//  for (int tmpRow = 0; tmpRow < columns; tmpRow++)
//  {
//    for (int tmpColumn = 0; tmpColumn < rows; tmpColumn++)
//    {
//      cell[tmpRow][tmpColumn].clear();
//    }
//  }
//}