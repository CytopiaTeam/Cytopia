#ifndef ENUMS_HXX_
#define ENUMS_HXX_

#include <stddef.h>

constexpr size_t LAYERS_COUNT = 7;

enum Layer : unsigned int
{
  BLUEPRINT,       // Optional layer - Map Blueprint
  PIPES,           // Optional layer - Pipes, Subway-pipes and so onn
  TERRAIN,         // Terrain tiles, decorations, ... - must always be a "full" tile
  MOVABLE_OBJECTS, // Cars, Pedestrians
  BUILDINGS,       // Buildings, Streets and everything that goes on the terrain
  ANIMATIONS,      // Animations like smoke
  SYMBOLS          // Symbols to display over buildings like no power / water and so on
};

#endif