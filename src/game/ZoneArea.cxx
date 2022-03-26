#include "ZoneArea.hxx"
#include "../engine/basics/PointFunctions.hxx"

void ZoneArea::spawnBuildings()
{
  // Implement spawnBuildings here
}

bool ZoneArea::isPartOfZone(Point coordinate)
{
  for (ZoneNode node : m_zoneNodes)
  {
    if (PointFunctions::isNeighborOf(coordinate, node.coordinate))
    {
      return true;
    }
  }
  return false;
}