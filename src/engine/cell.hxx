#include <vector>

#include "SDL2/SDL.h"


#include "basics/point.hxx";
#include "sprite.hxx";

#ifndef CELL_HXX_
#define CELL_HXX_


class Cell
{
public:
  Cell();
  Cell(Point isoCoordinates, Sprite* sprite);
  ~Cell();

  void setSprite(Sprite* sprite);
  Sprite* getSprite();

  void renderCell(Point cameraOffset, float zoom);

  void setNeighbors(std::vector<Cell*> neighbors);



private:
  Point _coordinates;
  Sprite* _sprite;

  std::vector<Cell*> _neighbors;

};


#endif