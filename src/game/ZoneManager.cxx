#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"
#include "../services/GameClock.hxx"
#include "../services/Randomizer.hxx"

ZoneManager::ZoneManager()
{
  GameClock::instance().addRealTimeClockTask(
      [this]()
      {
        spawnBuildings();
        return false;
      },
      1s, 1s);
}

// Nothing here right now
void ZoneManager::update() {}

void ZoneManager::spawnBuildings()
{
 for (auto zoneArea : m_zoneAreas)
 {
   zoneArea.spawnBuildings();
 }
}

void ZoneManager::addZoneNode(MapNode *node) { m_MapNodes.emplace_back(node); }

std::vector<int> ZoneManager::findAllSuitableZoneArea(const ZoneNode &zoneNode)
{
  std::vector<int> neighborZones;
  int i = 0;

  for (auto &zoneArea : m_zoneAreas)
  {
    if ((zoneArea.getZone() == zoneNode.zone) && zoneArea.isWithinBoundaries(zoneNode.coordinate) &&
        zoneArea.isNeighborOfZone(zoneNode.coordinate))
    {
      neighborZones.push_back(i);
    }

    ++i;
  }

  return neighborZones;
}

void ZoneManager::addZoneNode(Point coordinate, Zones zone, ZoneDensity ZoneDensity)
{
  // NOTE: Ignore density for agricultural zone
  // do
  ZoneNode newZone;
  newZone.coordinate = coordinate;
  newZone.zone = zone;
  newZone.ZoneDensity = ZoneDensity;

  auto zoneNeighbour = findAllSuitableZoneArea(newZone);

  if (zoneNeighbour.empty())
  {
    // new zonearea
    m_zoneAreas.emplace_back(newZone);
  }
  else if (zoneNeighbour.size() == 1)
  {
    // add to this zone
    m_zoneAreas[zoneNeighbour[0]].addZoneNode(newZone);
  }
  else
  {
    // merge zone areas
    ZoneArea &mergedZone = m_zoneAreas[zoneNeighbour[0]];
    mergedZone.addZoneNode(newZone);

    for (int idx = 1; idx < zoneNeighbour.size(); ++idx)
    {
      mergeZoneAreas(mergedZone, m_zoneAreas[zoneNeighbour[idx]]);
    }

    for (int idx = zoneNeighbour.size() - 1; idx > 0; --idx)
    {
      m_zoneAreas.erase(m_zoneAreas.begin() + zoneNeighbour[idx]);
    }
  }

  m_AllNodes.push_back(newZone);
}

void ZoneManager::removeZoneNode(MapNode *node)
{
  if (node)
  {
    LOG(LOG_INFO) << "Removing tiles does not yet work.";
    // m_MapNodes.erase(std::remove(m_MapNodes.begin(), m_MapNodes.end(), node));
  }
}

ZoneNode ZoneManager::getZoneNodeWithCoordinate(Point coordinate)
{
  for (const ZoneNode node : m_AllNodes)
  {
    if (node.coordinate == coordinate)
    {
      return node;
    }
  }

  // "This should not happen"
  //assert(false);
}