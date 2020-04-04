#ifndef ENUMS_HXX_
#define ENUMS_HXX_

#include <stddef.h>
#include "betterEnums.hxx"

enum Layer : unsigned int
{
  NONE,            // 0- this must be FIRST !!!
  BLUEPRINT,       // 1- Optional layer - Map Blueprint
  UNDERGROUND,     // 2- Optional layer - Pipes, Subway-pipes and so onn
  TERRAIN,         // 3- Terrain tiles, decorations, ... - must always be a "full" tile
  WATER,           // 4- Water tiles
  MOVABLE_OBJECTS, // 5- Cars, Pedestrians
  BUILDINGS,       // 6- Buildings, Streets and everything that goes on the terrain
  ANIMATIONS,      // 7- Animations like smoke
  SYMBOLS,         // 8- Symbols to display over buildings like no power / water and so on
  LAYERS_COUNT     // this must be LAST !!!
};

constexpr size_t FIRST_LAYER = NONE + 1;

/// This is a orderd list of all relevant layers we need to interact with
static const Layer allLayersOrdered[] = {Layer::TERRAIN, Layer::WATER, Layer::BUILDINGS, Layer::BLUEPRINT, Layer::UNDERGROUND};

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

/**
 * @brief LayerEditMode.
 * This enum is for switching between layers.
 * TERRAIN activates Terrain and buildings
 * BLUEPRINT activates blueprint + underground
 */
enum class LayerEditMode
{
  TERRAIN,
  BLUEPRINT
};

enum class PlacementMode
{
  SINGLE,
  STRAIGHT_LINE,
  LINE,
  RECTANGLE
};

#endif
