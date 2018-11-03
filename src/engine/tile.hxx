#ifndef TILE_HXX_
#define TILE_HXX_

#include "SDL2/SDL.h"
#include <bitset>

/** Tile Types as enum
  * @see TextureManager::tileTypesEnumToString
*/
enum class TileType
{
  TERRAIN,
  WATER,
  ROAD,
  RESIDENTIAL,
  COMMERCIAL,
  INDUSTRIAL
};

/** Tile Types as enum
* @see TextureManager::tileOrientationEnumToString
*/
enum class TileOrientation
{
  DEFAULT,
  CENTER,
  N,
  E,
  S,
  W,
  NE,
  NW,
  SE,
  SW,
  N_AND_E,
  N_AND_W,
  S_AND_E,
  S_AND_W
};

class Tile
{
public:
  Tile() = default;
  ~Tile() = default;

  static SDL_Texture *getTexture(TileType type, TileOrientation orientation);
  static TileOrientation caluclateOrientation(unsigned char bitMaskElevation);

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