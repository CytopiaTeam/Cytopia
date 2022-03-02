#ifndef ENUMS_HXX_
#define ENUMS_HXX_

#include <stddef.h>
#include "betterEnums.hxx"

/// All Layers we have
enum Layer : unsigned int
{
  NONE,              /// 0-  this must be FIRST !!!
  BLUEPRINT,         /// 1-  Optional layer - Map Blueprint
  UNDERGROUND,       /// 2-  Optional layer - Pipes, Subway-pipes and so onn
  TERRAIN,           /// 3-  Terrain tiles, decorations, ... - must always be a "full" tile
  ZONE,              /// 4-  Optional layer, zones(Industrial/Residential/Commercial).
  ROAD,              /// 5-  Optional layer, roads.
  WATER,             /// 6-  Water tiles
  MOVABLE_OBJECTS,   /// 7-  Cars, Pedestrians
  BUILDINGS,         /// 8-  Buildings, Streets and everything that goes on the terrain
  ANIMATIONS,        /// 9-  Animations like smoke
  SYMBOLS,           /// 10- Symbols to display over buildings like no power / water and so on
  GROUND_DECORATION, /// 11- Decoration to place beneath buildings. Like concrete or grass
  LAYERS_COUNT       /// this must be LAST !!!
};

/// This is a ordered list of all relevant layers we need to interact with
static Layer allLayersOrdered[] = {Layer::BLUEPRINT, Layer::UNDERGROUND, Layer::TERRAIN, Layer::WATER,
                                    Layer::GROUND_DECORATION, Layer::ZONE, Layer::ROAD,    Layer::BUILDINGS};

/**
 * @brief LayerEditMode.
 * This enum is for switching between layers.
 * TERRAIN activates Terrain and buildings
 * BLUEPRINT activates blueprint + underground
 */
enum class LayerEditMode
{
  TERRAIN,  /// Default "overworld" edit mode
  BLUEPRINT /// Placing water pipes and underground transportation on the Blueprint layer
};

enum class PlacementMode
{
  SINGLE,        /// Place tiles on a single spot
  STRAIGHT_LINE, /// Place tiles in a straight, rectangular line
  LINE,          /// Place tiles in a line from start to end point
  RECTANGLE      /// draw a rectangle between start and end point
};

enum class DemolishMode
{
  DEFAULT,          /// Demolish everything, but not
  DE_ZONE,          /// Remove only zones
  GROUND_DECORATION /// Remove only ground decoration
};

#endif
