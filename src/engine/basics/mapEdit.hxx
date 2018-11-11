#ifndef MAPEDIT_HXX_
#define MAPEDIT_HXX_

enum class TerrainEdit
{
  NONE,
  RAISE,
  LOWER,
  LEVEL,
  DEMOLISH
};

extern TerrainEdit terrainEditMode = TerrainEdit::NONE;

#endif