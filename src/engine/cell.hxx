#include <vector>
#include <map>

#include "SDL2/SDL.h"


#include "basics/point.hxx";
#include "sprite.hxx";

#ifndef CELL_HXX_
#define CELL_HXX_


class Cell
{
public:
  Cell();
  Cell(Point isoCoordinates, Sprite* sprite, SDL_Renderer* renderer, SDL_Window* window);
  ~Cell();

  void setSprite(Sprite* sprite);
  Sprite* getSprite();

  void renderCell(Point cameraOffset, float zoom);

  void setNeighbors(std::vector<Cell*> neighbors);
  void drawSurroundingTiles(Point isoCoordinates);
  void determineTile();

  void increaseHeight(int height);



private:
  Point _coordinates;
  Sprite* _sprite;

  std::vector<Cell*> _neighbors;
  SDL_Renderer* _renderer;
  SDL_Window* _window;

  bool hasElevatedNeighbors();


  int _tileID;

  // Bitmask for neighbor positions
  // [ T B L R ]
  // [ 0 0 0 0 ]

  // Our neighbor tiles look like this
  // 2 5 8  
  // 1 X 7
  // 0 3 6

  enum positions : unsigned char{
       TOP = 0x1,
       BOTTOM = 0x2,
       LEFT = 0x4,
       RIGHT = 0x8
  };

  unsigned char _position;


  // Map elvated tiles to tile ID.
  // not found means we can't draw a keytile, so elevate this tile!
  std::map<unsigned char, int> tileIdToPosition =
  {
    {TOP, 3},
    {BOTTOM, 5},
    {RIGHT, 1},
    {LEFT, 7},
    {BOTTOM | RIGHT, 2},
    {BOTTOM | LEFT, 8},
    {TOP | RIGHT, 0},
    {TOP | LEFT, 6},
    {TOP | LEFT, 6},

  };


};


#endif