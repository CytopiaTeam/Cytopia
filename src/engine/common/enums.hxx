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
  WATER,             /// 5-  Water tiles
  MOVABLE_OBJECTS,   /// 6-  Cars, Pedestrians
  BUILDINGS,         /// 7-  Buildings, Streets and everything that goes on the terrain
  ANIMATIONS,        /// 8-  Animations like smoke
  SYMBOLS,           /// 9-  Symbols to display over buildings like no power / water and so on
  GROUND_DECORATION, /// 10- Decoration to place beneath buildings. Like concrete or grass
  LAYERS_COUNT       /// this must be LAST !!!
};

constexpr size_t FIRST_LAYER = NONE + 1;

/// This is a ordered list of all relevant layers we need to interact with
static const Layer allLayersOrdered[] = {Layer::TERRAIN,   Layer::WATER,     Layer::GROUND_DECORATION, Layer::ZONE,
                                         Layer::BUILDINGS, Layer::BLUEPRINT, Layer::UNDERGROUND};

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
