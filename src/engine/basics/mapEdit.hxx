#ifndef MAPEDIT_HXX_
#define MAPEDIT_HXX_

#include <string>

enum class TerrainEdit
{
  NONE,
  RAISE,
  LOWER,
  LEVEL,
  DEMOLISH
};

extern TerrainEdit terrainEditMode;
extern std::string tileTypeEditMode;
extern bool demolishMode;

#endif