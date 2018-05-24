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
  void addCell(int row, int column, int z);
  std::shared_ptr<Cell> getCell(int x, int y);
  std::vector<std::shared_ptr<Cell>> getCellNeighbors(int x, int y);
  void initCells();


private:
  //std::vector< std::vector<std::shared_ptr<Cell>> > _cellMatrix;
  std::vector<std::shared_ptr<Cell> > _cellMatrix;

  
  int _columns;
  int _rows;
};