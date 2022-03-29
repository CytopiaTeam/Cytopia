#include "ZoneArea.hxx"
#include "../engine/basics/PointFunctions.hxx"

ZoneArea::ZoneArea(ZoneNode zoneNode)
    : m_zoneNodes{zoneNode}, xmin(std::max(0, zoneNode.coordinate.x - 1)), m_zone(zoneNode.zone),
      xmax(std::min(Settings::instance().mapSize, zoneNode.coordinate.x + 1)), ymin(std::max(0, zoneNode.coordinate.y - 1)),
      ymax(std::min(Settings::instance().mapSize, zoneNode.coordinate.y + 1))
{
}

void ZoneArea::spawnBuildings()
{
  // Implement spawnBuildings here
}

bool ZoneArea::isPartOfZone(Point coordinate)
{
  for (ZoneNode node : m_zoneNodes)
  {
    if (coordinate == node.coordinate)
    {
      return true;
    }
  }
  return false;
}

bool ZoneArea::isNeighborOfZone(Point coordinate) const
{
  for (ZoneNode node : m_zoneNodes)
  {
    if ((node.coordinate.x == coordinate.x) && (std::abs(node.coordinate.y - coordinate.y) <= 1) ||
        (node.coordinate.y == coordinate.y) && (std::abs(node.coordinate.x - coordinate.x) <= 1))
    {
      return true;
    }
  }

  return false;
}