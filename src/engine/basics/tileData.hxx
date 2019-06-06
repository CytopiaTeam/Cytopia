#ifndef TILEDATA_HXX_
#define TILEDATA_HXX_

#include <string>

/// min and max values for tileData
#define TD_PRICE_MIN 0
#define TD_PRICE_MAX 100000
#define TD_UPKEEP_MIN 0
#define TD_UPKEEP_MAX 10000
#define TD_POWER_MIN -100
#define TD_POWER_MAX 1000
#define TD_WATER_MIN -100
#define TD_WATER_MAX 1000
#define TD_POLLUTION_MIN -100
#define TD_POLLUTION_MAX 100
#define TD_CRIME_MIN -100
#define TD_CRIME_MAX 100
#define TD_HABITANTS_MIN 0
#define TD_HABITANTS_MAX 100
#define TD_FIREDANGER_MIN -100
#define TD_FIREDANGER_MAX 100
#define TD_TITLE_MAX_CHARS 100
#define TD_DESCRIPTION_MAX_CHARS 100
#define TD_BIOME_MAX_CHARS 100
#define TD_AUTHOR_MAX_CHARS 100
#define TD_ID_MAX_CHARS 100
#define TD_CATEGORY_MAX_CHARS 40

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
  std::string category;
  std::string biome; // the biome the tile belongs to
  TileSetData tiles;
  TileSetData cornerTiles;
  TileSetData slopeTiles;
  std::string title;
  std::string description;
  int price = 0;      // building cost
  int upkeepCost = 0; // monthly cost
  int power = 0;      // power production / consumption if negative
  int water = 0;      // water production / consumption if negative
  int tileIndex = -1; // index in the spritesheet, if it contains multiple tiles. (categories without "special handling")
  std::string author;
  bool drawGround = false;   // draw ground below sprite (grass, flowers, ...)
  bool placeOnGround = true; // wether or not this building is placeable on ground
  bool placeOnWater = false; // wether or not this building is placeable on ground
  int pollutionLevel = 0;    // Pollution this building produces or prevents
  int crimeLevel = 0;        // Crime this building produces or prevents (police station)
  int fireDangerLevel = 0;   // Fire Danger this building produces or prevents
  int habitants = 0;
};

#endif
