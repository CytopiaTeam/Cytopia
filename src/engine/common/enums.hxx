#ifndef ENUMS_HXX_
#define ENUMS_HXX_

#include <stddef.h>
#include "betterEnums.hxx"

constexpr size_t LAYERS_COUNT = 7;

enum Layer : unsigned int
{
  BLUEPRINT = 1,   // Optional layer - Map Blueprint
  UNDERGROUND,     // Optional layer - Pipes, Subway-pipes and so onn
  TERRAIN,         // Terrain tiles, decorations, ... - must always be a "full" tile
  WATER,           // Water tiles
  MOVABLE_OBJECTS, // Cars, Pedestrians
  BUILDINGS,       // Buildings, Streets and everything that goes on the terrain
  ANIMATIONS,      // Animations like smoke
  SYMBOLS,         // Symbols to display over buildings like no power / water and so on
  NONE             // If no layer is active
};

BETTER_ENUM(TileType, int,
            DEFAULT,    // Default is for buildings and practically everything that'll be placed on the TERRAIN layer
            TERRAIN,    // Terrain itself
            WATER,      // Water terrain
            BLUEPRINT,  // Same as terrain, but gets placed on the BLUEPRINT layer
            AUTOTILE,   // Autotiling to itself, like roads, power lines, etc
            UNDERGROUND // same as AUTOTILE, but for the BLUEPRINT layer
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

#endif
