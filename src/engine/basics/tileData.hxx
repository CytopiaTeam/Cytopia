#ifndef TILEDATA_HXX_
#define TILEDATA_HXX_

#include <string>
#include "enums.hxx"

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

BETTER_ENUM(TileType, int,
            DEFAULT,    // Default is for buildings and practically everything that'll be placed on the TERRAIN layer
            TERRAIN,    // Terrain itself
            WATER,      // Water terrain
            BLUEPRINT,  // Same as terrain, but gets placed on the BLUEPRINT layer
            AUTOTILE,   // Autotiling to itself, like roads, power lines, etc
            ZONE,       // Zones (rectangular placement)
            UNDERGROUND // same as AUTOTILE, but for the BLUEPRINT layer
)

//
BETTER_ENUM(Zones, int,
            NONE, // not applicable
            RESIDENTIAL, INDUSTRIAL, COMMERCIAL, AGRICULTURAL)

BETTER_ENUM(Wealth, int,
            NONE,   // not applicable
            LOW,    // Low income
            MEDIUM, // Medium income
            HIGH    // High income
)

BETTER_ENUM(Style, int,
            ALL,      // Default, place the Building in all Styles
            ASIAN,    // This building will only appear in a game with the Style Asian
            EUROPEAN, // This building will only appear in a game with the Style European
            US        // This building will only appear in a game with the Style US
)

struct TileSetData
{
  /**
  * count is the number of images that are in the tile set. This is for things that 
  * will have a random tile chosen from their set. 
  *
  * offset is where the first image in this tileset is, so a file could contain multiple
  * tilesets and offset would define where to start this tileset and count would define
  * how many images it has. offset = 0 is the first image, offset = 3 is the 4th tile. 
  *
  * rotations is the number of rotations that exist in this tileset (for buildings). 
  * this is not applicable for terrain and roads, their orientation is figured out
  * differently. For buildings that have multiple orientations, this isn't implemented
  * yet but it prevents buildings with multiple orientations from being placed with 
  * a random image (that might be the wrong size).
  **/
  std::string fileName;
  int count = 1;
  int clippingWidth = 0;
  int clippingHeight = 0;
  int offset = 0;    // The offset to start the spritesheet from in images (see above)
  int rotations = 1; // The number of rotation images in this spritesheet (see above)
};

struct RequiredTilesData
{
  unsigned int width;
  unsigned int height;
};

struct TileData
{
  std::string id;
  std::string category;
  std::string subCategory;
  uint8_t buildingsize; /// the size that the building occupies in tiles.
  std::string biome;    /// the biome the tile belongs to
  TileSetData tiles;
  TileType tileType = TileType::DEFAULT; /// todo: String for now, should be an enum
  TileSetData shoreTiles;
  TileSetData slopeTiles;
  std::string title;
  std::string description;
  int price = 0;      /// building cost
  int upkeepCost = 0; /// monthly cost
  int power = 0;      /// power production / consumption if negative
  int water = 0;      /// water production / consumption if negative
  Wealth wealth = +Wealth::LOW;
  bool drawGround = false;   /// draw ground below sprite (grass, flowers, ...)
  bool placeOnGround = true; /// wether or not this building is placeable on ground
  bool placeOnWater = false; /// whether or not this building is placeable on water
  bool isOverPlacable; /// Determines if other tiles can be placed over this one tile.
  int pollutionLevel = 0;  /// Pollution this building produces or prevents
  int crimeLevel = 0;      /// Crime this building produces or prevents (police station)
  int fireHazardLevel = 0; /// Fire Danger this building produces or prevents
  int inhabitants = 0; /// How many residents / workers this building can hold. Also how much jobs it provides
  int educationLevel = 0; ///How much education this building provides (educational building) / requires (job)
  std::vector<Zones> zones;
  std::vector<Style> style;
  std::string author;
  RequiredTilesData RequiredTiles;
};




#endif
