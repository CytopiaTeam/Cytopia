#include "cell.hxx"

Cell::Cell()
{

}

Cell::Cell(Point isoCoordinates, Sprite* sprite)
{
  _sprite = sprite;
  _coordinates = isoCoordinates;


}

Cell::~Cell()
{

}

void Cell::setSprite(Sprite* sprite)
{
  _sprite = sprite;
}

Sprite* Cell::getSprite()
{
  return _sprite;
}

void Cell::setNeighbors(std::vector<Cell*> neighbors)
{
  _neighbors = neighbors;
}

void Cell::renderCell(Point cameraOffset, float zoom)
{
  _sprite->render(cameraOffset, zoom);
}