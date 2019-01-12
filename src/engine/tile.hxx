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

struct TileData
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

enum TileList : size_t
{
  TILE_DEFAULT_ORIENTATION,
  TILE_E,
  TILE_N,
  TILE_N_AND_E,
  TILE_W,
  TILE_E_AND_W,
  TILE_N_AND_W,
  TILE_N_AND_E_AND_W,
  TILE_S,
  TILE_S_AND_E,
  TILE_N_AND_S,
  TILE_N_AND_E_AND_S,
  TILE_S_AND_W,
  TILE_S_AND_E_AND_W,
  TILE_N_AND_S_AND_W,
  TILE_ALL_DIRECTIONS
};

class TileManager
{
public:
  /// Retrieves instance of Singleton class TileManager
  static TileManager &instance()
  {
    static TileManager tileManager;
    return tileManager;
  }

  // Disable copy and assignemnt operators
  TileManager(TileManager const &) = delete;
  TileManager &operator=(TileManager const &) = delete;

  SDL_Texture *getTexture(const std::string &id, size_t tileMapType = 0);
  TileData *getTileData(const std::string &id);
  size_t caluclateSlopeOrientation(unsigned char bitMaskElevation);
  size_t caluclateTileOrientation(unsigned char bitMaskElevation);

  void init();

private:
  TileManager();
  ~TileManager() = default;

  std::unordered_map<std::string, TileData> tileData;
};

#endif
