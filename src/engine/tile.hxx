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

enum TileMap : size_t
{
  DEFAULT,
  CORNERS,
  SLOPES
};

enum TileSlopes : size_t
{
  N,
  W,
  E,
  S,
  NW,
  NE,
  SW,
  SE,
  N_AND_W,
  N_AND_E,
  S_AND_W,
  S_AND_E,
  BETWEEN,
  DEFAULT_ORIENTATION
};

class Tile
{
public:
  /// Retrieves Instance of Singleton class Texture Manager
  static Tile &Instance()
  {
    static Tile instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  Tile(Tile const &) = delete;
  Tile &operator=(Tile const &) = delete;

  SDL_Texture *getTexture(const std::string &id, size_t tileMapType = 0);
  TileInformation *getTileData(const std::string &id);
  std::string caluclateOrientation(unsigned char bitMaskElevation);
  size_t caluclateOrientationNew(unsigned char bitMaskElevation);

  void init();

private:
  Tile();
  ~Tile() = default;

  std::unordered_map<std::string, TileInformation> tileData;

  // empty TileInformation struct if a non-existing ID is beeing accessed
  TileInformation emptyTileData;
};

#endif
