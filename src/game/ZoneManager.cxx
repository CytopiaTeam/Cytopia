#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"
#include "../services/GameClock.hxx"
#include "../services/Randomizer.hxx"
#include "GameStates.hxx"

ZoneManager::ZoneManager()
{
  Engine::instance().map->registerCbPlaceBuilding(
      [this](const MapNode &mapNode)
      {
        // If we place a building on zone tile, occupy the node
        for (auto &zoneArea : m_zoneAreas)
        {
          zoneArea.occupyZoneNode(mapNode.getCoordinates());
        }
      });

  Engine::instance().map->registerCbPlaceZone(
      [this](const MapNode &mapNode)
      {
        // If we place a building on zone tile, occupy the node
     
        addZoneNode(mapNode.getCoordinates(), mapNode.getTileData(Layer::ZONE)->zones[0],
                      mapNode.getTileData(Layer::ZONE)->zoneDensity[0]);        
      });

  Engine::instance().map->registerCbDemolish(
      [this](const MapNode *mapNode)
      {
        switch (GameStates::instance().demolishMode)
        {
        case DemolishMode::DE_ZONE:
        {
          removeZoneNode(mapNode->getCoordinates());
          break;
        }
        case DemolishMode::DEFAULT:
        {
          for (auto &zoneArea : m_zoneAreas)
          {
            zoneArea.freeZoneNode(mapNode->getCoordinates());
          }
          break;
        }
        }
      });

  GameClock::instance().addRealTimeClockTask(
      [this]()
      {
        spawnBuildings();
        return false;
      },
      1s, 1s);
}

void ZoneManager::spawnBuildings()
{
  for (auto &zoneArea : m_zoneAreas)
  {
    // check if there are any buildings to spawn, if not, do nothing.
    if (zoneArea.isVacant())
    {
      zoneArea.spawnBuildings();
    }
  }
}

std::vector<int> ZoneManager::getAdjacentZoneAreas(const ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas)
{
  std::vector<int> neighborZones;
  int i = 0;

  for (auto &zoneArea : zoneAreas)
  {
    if (zoneArea.getZone() == zoneNode.zoneType && (zoneArea.getZoneDensity() == zoneNode.zoneDensity) &&
        zoneArea.isWithinBoundaries(zoneNode.coordinate) && zoneArea.isNeighborOfZone(zoneNode.coordinate))
    {
      neighborZones.push_back(i);
    }
    ++i;
  }

  return neighborZones;
}

void ZoneManager::addZoneNode(Point coordinate, ZoneType zoneType, ZoneDensity zoneDensity)
{
  ZoneNode newZone{coordinate, zoneType, zoneDensity, false};
  addZoneNodeToArea(newZone, m_zoneAreas);
}

void ZoneManager::addZoneNodeToArea(ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas)
{
  auto zoneNeighbour = getAdjacentZoneAreas(zoneNode, zoneAreas);

  if (zoneNeighbour.empty())
  {
    // new zonearea
    zoneAreas.emplace_back(zoneNode);
  }
  else if (zoneNeighbour.size() == 1)
  {
    // add to this zone
    zoneAreas[zoneNeighbour[0]].addZoneNode(zoneNode);
  }
  else
  {
    // merge zone areas
    ZoneArea &mergedZone = zoneAreas[zoneNeighbour[0]];
    mergedZone.addZoneNode(zoneNode);

    for (int idx = 1; idx < zoneNeighbour.size(); ++idx)
    {
      mergeZoneAreas(mergedZone, zoneAreas[zoneNeighbour[idx]]);
    }

    for (int idx = zoneNeighbour.size() - 1; idx > 0; --idx)
    {
      zoneAreas.erase(zoneAreas.begin() + zoneNeighbour[idx]);
    }
  }
}

std::vector<ZoneArea> ZoneManager::rebuildZoneArea(ZoneArea &zoneArea)
{
  std::vector<ZoneArea> newZoneAreas;

  for (ZoneNode zoneNode : zoneArea)
  {
    addZoneNodeToArea(zoneNode, newZoneAreas);
  }

  return newZoneAreas;
}

void ZoneManager::removeZoneNode(Point coordinate)
{
  for (auto zoneIt = m_zoneAreas.begin(); zoneIt != m_zoneAreas.end(); zoneIt++)
  {
    if (zoneIt->isWithinZone(coordinate))
    {
      zoneIt->removeZoneNode(coordinate);

      if (zoneIt->getSize() == 0)
      {
        m_zoneAreas.erase(zoneIt);
      }
      else
      {
        auto zoneAreas = rebuildZoneArea(*zoneIt);
        assert(zoneAreas.size() > 0);
        // If zoneAreas size is 1, means zoneArea is still compact, nothing to be done

        if (zoneAreas.size() > 1)
        {
          m_zoneAreas.erase(zoneIt);
          m_zoneAreas.insert(m_zoneAreas.end(), zoneAreas.begin(), zoneAreas.end());
        }
      }

      break;
    }
  }
}
