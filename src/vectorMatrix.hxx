#include <vector>
#include "sprite.hxx"

class vectorMatrix
{
public:
  vectorMatrix();
  vectorMatrix(int columns, int rows);
  ~vectorMatrix();
  
  void vectorMatrix::resizeMatrix(int rows, int columns);
  void addSprite(int row, int column, Sprite* sprite);
  void removeSprite(int x, int y);

  Sprite* getSprite(int x, int y);

private:
  std::vector< std::vector<Sprite*> > matrix;

};