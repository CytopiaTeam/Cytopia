#ifndef TILEDATA_HXX_
#define TILEDATA_HXX_

#include <string>
#include <vector>
#include "enums.hxx"
#include "../../util/Color.hxx"

/// min and max values for tileData
#define TD_PRICE_MIN 0
#define TD_PRICE_MAX 100000
#define TD_UPKEEP_MIN -10000
#define TD_UPKEEP_MAX 10000
#define TD_POWER_MIN -100
#define TD_POWER_MAX 1000
#define TD_WATER_MIN -100
#define TD_WATER_MAX 1000
#define TD_EDUCATION_MIN -100
#define TD_EDUCATION_MAX 100
#define TD_POLLUTION_MIN -100
#define TD_POLLUTION_MAX 100
#define TD_CRIME_MIN -100
#define TD_CRIME_MAX 100
#define TD_HABITANTS_MIN 0
#define TD_HABITANTS_MAX 10000
#define TD_FIREDANGER_MIN -100
#define TD_FIREDANGER_MAX 100
#define TD_HAPPINESS_MIN -100
#define TD_HAPPINESS_MAX 100
#define TD_TITLE_MAX_CHARS 100
#define TD_DESCRIPTION_MAX_CHARS 100
#define TD_BIOME_MAX_CHARS 100
#define TD_AUTHOR_MAX_CHARS 100
#define TD_ID_MAX_CHARS 100
#define TD_CATEGORY_MAX_CHARS 40
#define TD_SUBCATEGORY_MAX_CHARS 40
#define TD_REQUIREDTILES_MIN 1
#define TD_REQUIREDTILES_MAX 20

BETTER_ENUM(TileType, int,
            DEFAULT,          /// Default is for buildings and practically everything that'll be placed on the TERRAIN layer
            TERRAIN,          /// Terrain itself
            WATER,            /// Water terrain
            BLUEPRINT,        /// Same as terrain, but gets placed on the BLUEPRINT layer
            AUTOTILE,         /// Autotiling to itself, like roads, power lines, etc
            ZONE,             /// Zones (rectangular placement)
            ROAD,             /// Roads
            GROUNDDECORATION, /// Draw this Tile on GROUNDDECORATION layer. Buildings can be placed over it
            UNDERGROUND,      /// same as AUTOTILE, but for the BLUEPRINT layer
            RCI               /// Spawning automatically on RCI+ zones
)

//
BETTER_ENUM(Zones, int, RESIDENTIAL, INDUSTRIAL, COMMERCIAL, AGRICULTURAL)

BETTER_ENUM(Wealth, int,
            LOW,    /// Low income
            MEDIUM, /// Medium income
            HIGH    /// High income
)

BETTER_ENUM(Style, int,
            ASIAN,    /// This building will only appear in a game with the Style Asian
            EUROPEAN, /// This building will only appear in a game with the Style European
            US        /// This building will only appear in a game with the Style US
)

/**
 * This enum holds all data related to the TileSet (Spritesheet)
 **/
struct TileSetData
{
  std::string fileName; /// the filename of the spritesheet
  int count =
      1; /// count is the number of images that are in the tile set. This is for things that will have a random tile chosen from their set.
  int clippingWidth = 0;  /// the width of the clipRect. (Where the tile will be clipped from the spritesheet)
  int clippingHeight = 0; /// the height of the clipRect. (Where the tile will be clipped from the spritesheet)
  int offset =
      0; /// offset is where the first image in this tileset is, so a file could contain multiple tilesets and offset would define where to start this tileset and count would define how many images it has. offset = 0 is the first image, offset = 3 is the 4th tile.
  bool pickRandomTile = false; // determines if a random tile of the tileset should be used, if set to true
  int rotations =
      1; /// rotations is the number of rotations that exist in this tileset (for buildings).  this is not applicable for terrain and roads, their orientation is figured out differently. For buildings that have multiple orientations, this isn't implemented yet but it prevents buildings with multiple orientations from being placed with  a random image (that might be the wrong size).
};

/// How many tiles this building uses.
struct RequiredTilesData
{
  unsigned int width = 1;
  unsigned int height = 1;
};

/// Holds all releavted information to this specific tile
struct TileData
{
  std::string id;     /// 	The ID of this item. Must be unique and can be referenced in the code.
  std::string author; /// The author of this item
  std::string
      category; /// The category this item resides in. Categories are used for the building menu in-game and for sorting the items in the editors tree view
  std::string
      subCategory; /// subcategories are used for the building menu in-game and for sorting the items in the editors tree view
  uint8_t buildingsize; /// the size that the building occupies in tiles.
  std::vector<std::string>
      biomes; /// Restrict this building to spawn only in the given biomes. Must correspond to a biome defined in resources/data/TerrainGen.json
  std::vector<std::string> tags; /// Tags to filter buildings in the gui

  TileSetData tiles;                     ///Tile Spritesheet information
  TileType tileType = TileType::DEFAULT; /// todo: String for now, should be an enum
  TileSetData shoreTiles;                ///Shore Tile Spritesheet information
  TileSetData slopeTiles;                /// Slope  Tile Spritesheet information
  std::string title;                     /// The items title. It's shown ingame and in the editors tree-view
  std::string description;               /// Description of the item that is shown in it's details
  int price = 0;                         /// building cost
  int upkeepCost = 0;                    /// monthly cost
  int power = 0;                         /// power production / consumption if negative
  int water = 0;                         /// water production / consumption if negative

  std::vector<std::string>
      groundDecoration; /// tileID of the item that should be drawn on ground below sprite instead of terrain(grass, concrete, ...). Must be a tileID with tileType GroundDecoration
  bool placeOnGround = true;     /// wether or not this building is placeable on ground
  bool placeOnWater = false;     /// whether or not this building is placeable on water
  bool isOverPlacable;           /// Determines if other tiles can be placed over this one tile.
  int pollutionLevel = 0;        /// Pollution this building produces or prevents
  int crimeLevel = 0;            /// Crime this building produces or prevents (police station)
  int fireHazardLevel = 0;       /// Fire Danger this building produces or prevents
  int inhabitants = 0;           /// How many residents / workers this building can hold. Also how much jobs it provides
  int happiness = 0;             /// The effect on happiness around this building.
  int educationLevel = 0;        /// How much education this building provides (educational building) / requires (job)
  std::vector<Zones> zones;      /// Restrict this building to a zone type.
  std::vector<Style> style;      /// Restrict this building to certain Art Styles.
  std::vector<Wealth> wealth;    /// Restrict this building to a certain wealth level. See enum Wealth
  std::vector<RGBAColor> colors; /// Possible color variations for this tile (magic pixel colors)

  RequiredTilesData RequiredTiles; /// How many tiles this building uses.
};

#endif
