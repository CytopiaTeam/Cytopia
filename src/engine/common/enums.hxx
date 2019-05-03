#ifndef ENUMS_HXX_
#define ENUMS_HXX_

#include "../../ThirdParty/betterEnums.hxx"

// BLUEPRINT - Optional layer - Map Blueprint
// PIPES - Optional layer - Pipes, Subway-pipes and so onn
// TERRAIN - Terrain tiles, decorations, ... - must always be a "full" tile
// MOVABLE_OBJECTS - Cars, Pedestrians
// DRAW_ON_GROUND - Tiles that have drawOnGround set to true
// ANIMATIONS - Animations like smoke
// SYMBOLS - Symbols to display over buildings like no power / water and so on
BETTER_ENUM(Layer, int, BLUEPRINT, PIPES, TERRAIN, MOVABLE_OBJECTS, DRAW_ON_GROUND, ANIMATIONS, SYMBOLS)

#endif