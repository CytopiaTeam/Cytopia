#include "vectorMatrix.hxx"

vectorMatrix::vectorMatrix()
{

}

vectorMatrix::vectorMatrix(int columns, int rows)
  : matrix(rows+1, std::vector<Sprite*>(columns+1))
{

}

vectorMatrix::~vectorMatrix()
{

}


void vectorMatrix::resizeMatrix(int rows, int columns)
{
  matrix.resize(rows + 1);
  for (auto &it : matrix)
  {
    it.resize(columns + 1);
  }
}

void vectorMatrix::addSprite(int x, int y, Sprite* sprite)
{
  matrix[x][y] = sprite;
}

Sprite* vectorMatrix::getSprite(int x, int y)
{
  return matrix[x][y];
}

void vectorMatrix::removeSprite(int x, int y)
{
  matrix[x][y] = nullptr;
}