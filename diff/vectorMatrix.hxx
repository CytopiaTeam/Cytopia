#include <vector>

#include "sprite.hxx"

class vectorMatrix
{
  std::vector<Sprite> **cell;

  int columns;
  int rows;

public:
  vectorMatrix(int columns, int rows);
  virtual ~vectorMatrix();

  void addCellAt(int row, int column, const Sprite& entry);

  virtual std::vector<Sprite>* getCell(int row, int column);

  void clearMatrix();
};