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

  enum positions : unsigned int{
       NOTHING = 0x0,
       TOP = 0x1,
       BOTTOM = 0x2,
       LEFT = 0x4,
       RIGHT = 0x8,
       TOP_LEFT = 0x10,
       TOP_RIGHT = 0x20,
       BOTTOM_LEFT = 0x40,
       BOTTOM_RIGHT = 0x80,

       ELEVATE = RIGHT | LEFT
  };

  unsigned int _position;


  // Map elvated tiles to tile ID.
  // not found means we can't draw a keytile, so elevate this tile!
  std::map<unsigned int, int> keyTileMap =
  {
    { NOTHING, 14 },
    { TOP, 3 },
    { BOTTOM, 5 },
    { RIGHT, 1 },
    { LEFT, 7 },
    { BOTTOM_RIGHT, 2 },
    { BOTTOM_LEFT, 8 },
    { TOP_RIGHT, 0 },
    { TOP_LEFT, 6 },

    { TOP_LEFT | TOP_RIGHT, 3 },
    { TOP | TOP_LEFT, 3 },
    { TOP | TOP_RIGHT, 3 },
    { TOP | TOP_RIGHT | TOP_LEFT, 3 },

    { BOTTOM_LEFT | BOTTOM_RIGHT, 5 },
    { BOTTOM | BOTTOM_LEFT, 5 },
    { BOTTOM | BOTTOM_RIGHT, 5 },
    { BOTTOM | BOTTOM_RIGHT | BOTTOM_LEFT, 5 },
    
    { TOP_LEFT | BOTTOM_LEFT, 7},
    { LEFT | BOTTOM_LEFT, 7},
    { LEFT | TOP_LEFT, 7},
    { LEFT | TOP_LEFT | BOTTOM_LEFT, 7},

    { BOTTOM_RIGHT | TOP_RIGHT, 1 },
    { BOTTOM_RIGHT | RIGHT, 1 },
    { TOP_RIGHT | RIGHT, 1 },
    { RIGHT | TOP_RIGHT | BOTTOM_RIGHT, 1 },

    { TOP | LEFT, 9 },
    { TOP | LEFT | BOTTOM_LEFT, 9 },
    { TOP | LEFT | TOP_RIGHT, 9 },
    { TOP | LEFT | TOP_LEFT, 9 },
    { TOP | LEFT | TOP_LEFT | TOP_RIGHT, 9 },
    { TOP | LEFT | TOP_LEFT | BOTTOM_LEFT, 9 },
    { TOP | LEFT | TOP_LEFT | BOTTOM_LEFT | TOP_RIGHT, 9 },
    
    { TOP | RIGHT, 10 },
    { TOP | RIGHT | BOTTOM_RIGHT, 10 },
    { TOP | RIGHT | TOP_LEFT, 10 },
    { TOP | RIGHT | TOP_RIGHT, 10 },
    { TOP | RIGHT | TOP_RIGHT | TOP_LEFT, 10 },
    { TOP | RIGHT | TOP_RIGHT | BOTTOM_RIGHT, 10 },
    { TOP | RIGHT | TOP_RIGHT | TOP_LEFT | BOTTOM_RIGHT, 10 },

    { BOTTOM | RIGHT, 13 },
    { BOTTOM | RIGHT | TOP_RIGHT, 13 },
    { BOTTOM | RIGHT | BOTTOM_LEFT, 13 },
    { BOTTOM | RIGHT | BOTTOM_RIGHT, 13 },
    { BOTTOM | RIGHT | BOTTOM_RIGHT | BOTTOM_LEFT , 13 },
    { BOTTOM | RIGHT | BOTTOM_RIGHT | TOP_RIGHT , 13 },
    { BOTTOM | RIGHT | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_RIGHT , 13 },
    
    { BOTTOM | LEFT, 11 },
    { BOTTOM | LEFT | TOP_LEFT, 11 },
    { BOTTOM | LEFT | BOTTOM_RIGHT, 11 },
    { BOTTOM | LEFT | BOTTOM_LEFT, 11 },
    { BOTTOM | LEFT | BOTTOM_LEFT | TOP_LEFT, 11 },
    { BOTTOM | LEFT | BOTTOM_LEFT | BOTTOM_RIGHT, 11 },
    { BOTTOM | LEFT | BOTTOM_LEFT | TOP_LEFT | BOTTOM_RIGHT, 11 },

    //diagonal tiles - insert a block
    { TOP_LEFT | BOTTOM_RIGHT, 4},
    { BOTTOM_LEFT | TOP_RIGHT, 4},

  };
};


#endif