#ifndef TILE_HXX_
#define TILE_HXX_

#include "SDL2/SDL.h"

#include <unordered_map>
#include <string>

struct TileSetData
{
  std::string fileName;
  int count = 1;
  int clippingWidth = 0;
  int clippingHeight = 0;
};

struct TileInformation
{
  std::string id;
  std::string type;
  TileSetData tiles;
  TileSetData cornerTiles;
  TileSetData slopeTiles;
  std::string title;
  std::string description;
  int price = 0;      // building cost
  int upkeepCost = 0; // monthly cost
  int power = 0;      // power production / consumption if negative
  int water = 0;      // water production / consumption if negative
  std::string author;
  bool drawGround = false; // draw ground below sprite (grass, flowers, ...)
};

class Tile
{
public:
  Tile() = delete;
  ~Tile() = delete;

  static SDL_Texture *getTexture(const std::string &type, const std::string &orientation);
  static std::string caluclateOrientation(unsigned char bitMaskElevation);

  static void init();

private:
  static std::unordered_map<std::string, TileInformation> tileData;
};

#endif
