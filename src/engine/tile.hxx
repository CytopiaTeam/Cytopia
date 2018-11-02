#ifndef TILE_HXX_
#define TILE_HXX_

#include "SDL2/SDL.h"
#include <bitset>

class Tile
{
public:
  Tile() = default;
  ~Tile() = default;

  SDL_Texture *getTexture(std::string type, std::string orientation);
  std::string caluclateOrientation(unsigned char bitMaskElevation);

private:
  enum elevatedPosition : unsigned char
  {
    NO_NEIGHBORS = 0,
    ELEVATED_TOP = 1,
    ELEVATED_BOTTOM = 2,
    ELEVATED_LEFT = 4,
    ELEVATED_RIGHT = 8,
    ELEVATED_TOP_LEFT = 10,
    ELEVATED_TOP_RIGHT = 20,
    ELEVATED_BOTTOM_LEFT = 40,
    ELEVATED_BOTTOM_RIGHT = 80,
    ELEVATED_TOP_AND_RIGHT = 9
  };
};

#endif