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
  //   constexpr int amountOfBuildingsToSpawn = 5;
  //   auto &randomizer = Randomizer::instance();
  //   // shuffle mapNodes so placement of building looks random
  //   randomizer.shuffle(m_MapNodes.begin(), m_MapNodes.end());

  //   int buildingsSpawned = 0;
  //   // pick every single zone node we have
  //   for (auto &node : m_MapNodes)
  //   {
  //     if (buildingsSpawned >= amountOfBuildingsToSpawn)
  //     {
  //       break;
  //     }
  //     // if a building is bigger than one node, the adjacent nodes will no longer be a zone node and buildings can't be spawned there
  //     // consider that we're in a loop and if the zones stay, they will still be in the vector we're iterating over.
  //     if (!node || !node->getTileData(Layer::ZONE))
  //     {
  //       continue; // if the node that is still in the vector is no longer a zone node, skip
  //     }

  //     // a building can be tied to multiple zones. So get all elligible zones for this building
  //     std::vector<std::string> availableZoneTiles;

  //     Zones thisZone = Zones::RESIDENTIAL; // initialize zone with residential. Maybe add invalid later?
  //     // a zone tile only has one zone tied to it. the one it represents, so pick first element of vector
  //     if (node->getTileData(Layer::ZONE)->zones[0])
  //     {
  //       thisZone = node->getTileData(Layer::ZONE)->zones[0];
  //     }

  //     // get the maximum size we can spawn at this node, but limit it by 4x4 tiles
  //     unsigned int maxSizeX = std::min(4, static_cast<int>(getMaximumTileSize(node->getCoordinates()).width));
  //     unsigned int maxSizeY = std::min(4, static_cast<int>(getMaximumTileSize(node->getCoordinates()).height));
  //     TileSize maxTileSize = {maxSizeX, maxSizeY};

  //     std::string building = TileManager::instance().getRandomTileIDForZoneWithRandomSize(thisZone, maxTileSize);

  //     // place the building
  //     std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), building);
  //     Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), building, false);
  //     buildingsSpawned++;
  // }
}

void ZoneManager::addZoneNode(MapNode *node) { m_MapNodes.emplace_back(node); }

std::vector<ZoneArea *> ZoneManager::findAllSutableZoneArea(const ZoneNode &zoneNode)
{
  std::vector<ZoneArea *> neighborZones;
  for (auto &zoneArea : m_zoneAreas)
  {
    if ((zoneArea.getZone() == zoneNode.zone) && zoneArea.isWithinBoundaries(zoneNode.coordinate) && zoneArea.isNeighborOfZone(zoneNode.coordinate))
    {
      neighborZones.push_back(&zoneArea);
    }
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

  auto zoneNeighbour = findAllSutableZoneArea(newZone);

  if (zoneNeighbour.empty())
  {
    // new zonearea
    m_zoneAreas.emplace_back(newZone);
  }
  else if (zoneNeighbour.size() == 1)
  {
    // add to this zone
    zoneNeighbour[0]->addZoneNode(newZone);
  }
  else
  {
    // mergezones
  }

  m_AllNodes.push_back(newZone);
}

void ZoneManager::rebuildZoneAreas()
{
  //// just a stub imoplementation to add one single area and all the nodes we have to it
  //for (auto node : m_AllNodes)
  //{
  //  // if t here is no zone area yet, create the first one and add the current node.
  //  ZoneArea newZoneArea;
  //  LOG(LOG_INFO) << "Size " << m_zoneAreas.size();
  //  if (m_zoneAreas.size() == 0)
  //  {
  //    newZoneArea.addZoneNode(node);
  //    m_zoneAreas.push_back(newZoneArea);
  //    continue;
  //  }

  //  bool scanMoreNeighbors = true;
  //  // now we scan all neighbors of our current loop until we don't find more neighbors.
  //  int distance = 0;
  //  LOG(LOG_INFO) << "neihbor+1";
  //  while (scanMoreNeighbors)
  //  {
  //    bool nextIteration = false;
  //    distance++;
  //    for (auto coords : PointFunctions::getNeighbors(node.coordinate, false, distance))
  //    {
  //      LOG(LOG_INFO) << "Distance: " << distance;
  //      if (nextIteration)
  //        LOG(LOG_INFO) << "next iteration = true";
  //      for (auto zoneArea : m_zoneAreas)
  //      {
  //        // LOG(LOG_INFO) << "scanning neighbor";
  //        if (!zoneArea.isPartOfZone(coords))
  //          LOG(LOG_INFO) << "it is not part of an existing zone";
  //        if (zoneArea.isNeighborOfZone(coords))
  //        {
  //          LOG(LOG_INFO) << coords.x << ", " << coords.y << "it is a neighbor of an existing zone - adding it";

  //          zoneArea.addZoneNode(getZoneNodeWithCoordinate(coords));
  //          nextIteration = true;
  //          break;
  //        }
  //      }
  //    }
  //    if (!nextIteration)
  //    {
  //      scanMoreNeighbors = false;
  //    }
  //  }
  //  if (newZoneArea.getSize() != 0)
  //  {
  //    m_zoneAreas.push_back(newZoneArea);
  //  }
  //}

  //LOG(LOG_INFO) << "Found areas: " << m_zoneAreas.size();
  //for (auto zone : m_zoneAreas)
  //{
  //  int it = 1;
  //  LOG(LOG_INFO) << "Zone " << it << " - size: " << zone.getSize();
  //  it++;
  //}
}

void ZoneManager::removeZoneNode(MapNode *node)
{
  if (node)
  {
    LOG(LOG_INFO) << "Removing tiles does not yet work.";
    // m_MapNodes.erase(std::remove(m_MapNodes.begin(), m_MapNodes.end(), node));
  }
}

void ZoneManager::getNodeArea(MapNode *node)
{
  LOG(LOG_INFO) << "Starting at " << node->getCoordinates().x << ", " << node->getCoordinates().y;

  for (auto coord : PointFunctions::getNeighbors(node->getCoordinates(), true, 2))
  {
    LOG(LOG_INFO) << "Found " << coord.x << ", " << coord.y;
  }
  if (node)
  {
    int nodeX = node->getCoordinates().x;
    int nodeY = node->getCoordinates().y;

    std::vector<Point> areaCoordinates;
    LOG(LOG_INFO) << "Starting ";

    int dist = 1;
    for (int x = nodeX - dist; x <= nodeX + dist; x++)
    {
      for (int y = nodeY - dist; y <= nodeY + dist; y++)
      {

        // LOG(LOG_INFO) << "Found " << x << ", " << y;
      }
    }
  }
}

// TileSize ZoneManager::getMaximumTileSize(Point originPoint)
// {
//   TileSize possibleSize;

//   for (int distance = 1; distance <= possibleSize.width || distance <= possibleSize.height; distance++)
//   {
//     const MapNode *currentNodeInXDirection = getZoneNodeWithCoordinate({originPoint.x - distance, originPoint.y});
//     const MapNode *currentNodeInYDirection = getZoneNodeWithCoordinate({originPoint.x, originPoint.y + distance});

//     if (currentNodeInXDirection && currentNodeInXDirection->getTileData(Layer::ZONE))
//     {
//       possibleSize.width++;
//     }
//     if (currentNodeInYDirection && currentNodeInYDirection->getTileData(Layer::ZONE))
//     {
//       possibleSize.height++;
//     }
//   }
//   return possibleSize;
// }

// const MapNode *ZoneManager::getZoneNodeWithCoordinate(Point coordinate)
// {
//   for (const MapNode *node : m_MapNodes)
//   {
//     if (node->getCoordinates().x == coordinate.x && node->getCoordinates().y == coordinate.y)
//     {
//       return node;
//     }
//   }
//   return nullptr;
// }

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
  //6assert(false);
}