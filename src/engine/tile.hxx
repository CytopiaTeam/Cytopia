#ifndef TILE_HXX_
#define TILE_HXX_

#include "SDL2/SDL.h"
#include <bitset>

class Tile
{
public:
  Tile() = default;
  ~Tile() = default;

  static SDL_Texture *getTexture(const std::string &type, const std::string &orientation);
  static std::string caluclateOrientation(unsigned char bitMaskElevation);

};

#endif