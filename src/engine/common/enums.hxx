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
            UNDERGROUND // same as AUTOTILE, but for the BLUEPRINT layer
)

#endif
