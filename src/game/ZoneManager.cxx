#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"
#include "../services/GameClock.hxx"
#include "../services/Randomizer.hxx"
#include "GameStates.hxx"

ZoneManager::ZoneManager()
{
  if (!Engine::instance().map)
  {
    LOG(LOG_ERROR) << "ZoneManager() constructor has been called with no valid map object available!";
  }

  Engine::instance().map->registerCallbackFunction(
      [this](const MapNode &mapNode)
      {
        // If we place a zone tile, add it to the ZoneManager
        if (mapNode.getTileData(Layer::ZONE) && !mapNode.getTileData(Layer::ZONE)->zones.empty() &&
            !mapNode.getTileData(Layer::ZONE)->zoneDensity.empty())
        {
          addZoneNode(mapNode.getCoordinates(), mapNode.getTileData(Layer::ZONE)->zones[0],
                      mapNode.getTileData(Layer::ZONE)->zoneDensity[0]);
        }
        if (mapNode.getTileData(Layer::BUILDINGS) && mapNode.getTileData(Layer::ZONE))
        {
          for (auto zoneArea : m_zoneAreas)
          {
            zoneArea.occupyZoneNode(mapNode.getCoordinates());
            int occupied = 0;
            for (auto node : zoneArea.m_zoneNodes)
            {
              if (node.occupied)
                occupied++;
            }
            // TODO: This still has debug stuff in it.
            LOG(LOG_INFO) << "occupied nodes: " << occupied << " of " << zoneArea.getSize();
          }
        }
      });

  Engine::instance().map->registerCallbackFunction(
      [this](const MapNode *mapNode)
      {
        // If we are in dezone mode and  the tile has a zone tile, remove it from the ZoneManager
        if (mapNode->getTileData(Layer::ZONE))
        {
          // TODO: zones can't be removed automatically, so merge those if zone removal on spawning is fixed
          if (GameStates::instance().demolishMode == DemolishMode::DE_ZONE)
          {
            LOG(LOG_DEBUG) << "dezone - removing a a zone node";
            for (auto zoneArea : m_zoneAreas)
            {
              zoneArea.freeZoneNode(mapNode->getCoordinates());
            }
          }
          else
          {
            LOG(LOG_DEBUG) << "removing a a zone node without dezone";
            for (auto zoneArea : m_zoneAreas)
            {
              zoneArea.freeZoneNode(mapNode->getCoordinates());
            }
          }
        }

        // If we are in demolish mode and the tile is a building that is on a zone tile, remove it from the empty zone ZoneManager
        if (GameStates::instance().demolishMode == DemolishMode::DEFAULT && mapNode->getTileData(Layer::BUILDINGS) &&
            mapNode->getTileData(Layer::ZONE))
        {
          LOG(LOG_DEBUG) << "demolish a building on a zone node";
          removeZoneNode(mapNode->getCoordinates());
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
  for (auto zoneArea : m_zoneAreas)
  {
    // check if there are any buildings to spawn, if not, do nothing.
    if (zoneArea.getSize() != 0)
    {
      zoneArea.spawnBuildings();
    }
    // LOG(LOG_INFO) << "zone area size: " << zoneArea.getSize();
    // LOG(LOG_INFO) << "all nodes: " << m_AllNodes.size();
  }
  // LOG(LOG_DEBUG) << " Number of areas: " << m_zoneAreas.size();
}

std::vector<int> ZoneManager::findAllSuitableZoneArea(const ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas)
{
  std::vector<int> neighborZones;
  int i = 0;

  for (auto &zoneArea : zoneAreas)
  {
    if (zoneArea.getZone() == zoneNode.zone && (zoneArea.getZoneDensity() == zoneNode.zoneDensity) &&
        zoneArea.isWithinBoundaries(zoneNode.coordinate) && zoneArea.isNeighborOfZone(zoneNode.coordinate))
    {
      neighborZones.push_back(i);
    }
    ++i;
  }

  return neighborZones;
}

void ZoneManager::addZoneNode(Point coordinate, Zones zone, ZoneDensity zoneDensity)
{
  // NOTE: Ignore density for agricultural zone
  ZoneNode newZone{coordinate, zone, zoneDensity, false};
  addZoneNodeToArea(newZone, m_zoneAreas);
  m_AllNodes.push_back(newZone);
}

void ZoneManager::addZoneNodeToArea(ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas)
{
  auto zoneNeighbour = findAllSuitableZoneArea(zoneNode, zoneAreas);

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
    if (zoneIt->isPartOfZone(coordinate))
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

  // remove if should remove elements from the vector
  m_AllNodes.erase(std::remove_if(m_AllNodes.begin(), m_AllNodes.end(),
                                  [coordinate](const ZoneNode &zone) { return zone.coordinate == coordinate; }),
                   m_AllNodes.end());
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