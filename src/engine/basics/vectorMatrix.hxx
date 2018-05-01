#include <vector>
#include "../sprite.hxx"
#include "../cell.hxx"

class vectorMatrix
{
public:
  vectorMatrix();
  vectorMatrix(int columns, int rows);
  ~vectorMatrix();
  
  void vectorMatrix::resizeMatrix(int rows, int columns);
  void addSprite(int row, int column, Sprite* sprite);
  void removeSprite(int x, int y);
  void vectorMatrix::clearMatrix();

  // CELL
  void addCell(int row, int column, Cell* cell);
  Cell* getCell(int x, int y);
  std::vector<Cell*> setCellNeighbors(int x, int y);


  std::vector<Sprite*> vectorMatrix::findNeighbors(int x, int y);
  Sprite* getSprite(int x, int y);

private:
  std::vector< std::vector<Sprite*> > _matrix;
  std::vector< std::vector<Cell*> > _cellMatrix;
  int _columns;
  int _rows;
  int _neighborCount;
};