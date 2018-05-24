#include <vector>
#include "../sprite.hxx"
#include "../cell.hxx"

class vectorMatrix
{
public:
  vectorMatrix() = default;
  vectorMatrix(int columns, int rows);
  ~vectorMatrix() = default;
  
  void resizeMatrix(int rows, int columns);

  // CELL
  void addCell(int x, int y, int z) { _cellMatrix[x * _columns + y] = std::shared_ptr<Cell>(new Cell(Point(x, y, z))); };
  std::shared_ptr<Cell> getCell(int x, int y) { return _cellMatrix[x * _columns + y]; };
  std::vector<std::shared_ptr<Cell>> getCellNeighbors(int x, int y);
  void initCells();


private:
  std::vector<std::shared_ptr<Cell> > _cellMatrix;
  
  int _columns;
  int _rows;
};