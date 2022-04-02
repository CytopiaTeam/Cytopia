#include "ZoneArea.hxx"
#include "../engine/basics/PointFunctions.hxx"
#include "../services/Randomizer.hxx"
#include "../engine/TileManager.hxx"
#include "../engine/Engine.hxx"

void mergeZoneAreas(ZoneArea &mainZone, ZoneArea &toBeMerged)
{
  mainZone.m_zoneNodes.insert(mainZone.m_zoneNodes.end(), toBeMerged.m_zoneNodes.begin(), toBeMerged.m_zoneNodes.end());
  mainZone.m_hasPower |= toBeMerged.m_hasPower;
  mainZone.m_hasWater |= toBeMerged.m_hasWater;
  mainZone.xmin = std::min(mainZone.xmin, toBeMerged.xmin);
  mainZone.xmax = std::max(mainZone.xmax, toBeMerged.xmax);
  mainZone.ymin = std::min(mainZone.ymin, toBeMerged.ymin);
  mainZone.ymax = std::max(mainZone.ymax, toBeMerged.ymax);
};

ZoneArea::ZoneArea(ZoneNode zoneNode)
    : m_zoneNodes{zoneNode}, xmin(std::max(0, zoneNode.coordinate.x - 1)), m_zone(zoneNode.zone),
      m_zoneDensity(zoneNode.zoneDensity), xmax(std::min(Settings::instance().mapSize, zoneNode.coordinate.x + 1)),
      ymin(std::max(0, zoneNode.coordinate.y - 1)), ymax(std::min(Settings::instance().mapSize, zoneNode.coordinate.y + 1))
{
}

void ZoneArea::spawnBuildings()
{

  constexpr int amountOfBuildingsToSpawn = 5;
  auto &randomizer = Randomizer::instance();
  // shuffle mapNodes so placement of building looks random
  randomizer.shuffle(m_zoneNodes.begin(), m_zoneNodes.end());

  int buildingsSpawned = 0;

  //   auto it = std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),[](const ZoneNode& node) {
  //     return node.occupied == false;
  // });
  // if (it != m_zoneNodes.end())
  // {
  //   LOG(LOG_INFO) << "Free building at " << (*it).coordinate.x << ", " << (*it).coordinate.y;
  //   // std::cout <<  (*it).coordinate.x << "\n";
  // }

  // pick every single zone node we have
  for (auto &node : m_zoneNodes)
  {
    if (node.occupied)
    {
      continue;
    }
#
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }

    // !! --- -IMPORTANT ----- check if neighboring node is already occupied
    // if a building is bigger than one node, the adjacent nodes will no longer be a zone node and buildings can't be spawned there
    // consider that we're in a loop and if the zones stay, they will still be in the vector we're iterating over.
    // if (!node || !node->getTileData(Layer::ZONE))
    // {
    //   continue; // if the node that is still in the vector is no longer a zone node, skip
    // }

    // a building can be tied to multiple zones. So get all elligible zones for this building
    std::vector<std::string> availableZoneTiles;

    // get the maximum size we can spawn at this node, but limit it by 4x4 tiles
    unsigned int maxSizeX = std::min(4, static_cast<int>(getMaximumTileSize(node.coordinate).width));
    unsigned int maxSizeY = std::min(4, static_cast<int>(getMaximumTileSize(node.coordinate).height));
    TileSize maxTileSize = {maxSizeX, maxSizeY};

    std::string building = TileManager::instance().getRandomTileIDForZoneWithRandomSize(getZone(), getZoneDensity(), maxTileSize);

    // place the building
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node.coordinate, building);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), building, false);
    buildingsSpawned++;
  }
}

bool ZoneArea::isPartOfZone(Point coordinate)
{
  return m_zoneNodes.end() != std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),
                                           [&coordinate](const ZoneNode &node) { return node.coordinate == coordinate; });
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

TileSize ZoneArea::getMaximumTileSize(Point originPoint)
{
  TileSize possibleSize;

  for (int distance = 1; distance <= possibleSize.width || distance <= possibleSize.height; distance++)
  {
    // check if there's a tile in x direction (top of the origin point)
    if (isPartOfZone({originPoint.x - distance, originPoint.y}))
    {
      possibleSize.width++;
    }
    // check if there's a tile in y direction (right of the origin point)
    if (isPartOfZone({originPoint.x, originPoint.y + distance}))
    {
      possibleSize.height++;
    }
  }
  return possibleSize;
}

void ZoneArea::addZoneNode(ZoneNode zoneNode)
{

  //   if (std::find(m_zoneNodes.begin(), m_zoneNodes.end(), zoneNode) != m_zoneNodes.end()
  //   {return;}
  // )
  // if (std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),
  //                                  [zoneNode](const ZoneNode &zone) { return zone.coordinate == zoneNode.coordinate; }),
  //                   m_zoneNodes.end())

  // TODO: need to only add a zonenode if it is not already in the vector
  m_zoneNodes.push_back(zoneNode);

  if (zoneNode.coordinate.x == xmin)
  {
    xmin = std::max(0, xmin - 1);
  }
  else if (zoneNode.coordinate.x == xmax)
  {
    xmax = std::min(Settings::instance().mapSize, xmax + 1);
  }
  else if (zoneNode.coordinate.y == ymin)
  {
    ymin = std::max(0, ymin - 1);
  }
  else if (zoneNode.coordinate.y == ymax)
  {
    ymax = std::min(Settings::instance().mapSize, ymax + 1);
  }
}

void ZoneArea::removeZoneNode(Point coordinate)
{
  LOG(LOG_INFO) << "ZoneArea::removeZoneNode - " << coordinate.x << ", " << coordinate.y;

  LOG(LOG_DEBUG) << "Before removal. Size" << getSize();

  m_zoneNodes.erase(std::remove_if(m_zoneNodes.begin(), m_zoneNodes.end(),
                                   [coordinate](const ZoneNode &zone) { return zone.coordinate == coordinate; }),
                    m_zoneNodes.end());

  LOG(LOG_DEBUG) << "After removal. Size" << getSize();

  // for (auto it = m_zoneNodes.begin(); it != m_zoneNodes.end(); /* NOTHING */)
  // {
  //   if ((*it).coordinate == coordinate)
  //   {
  //     LOG(LOG_DEBUG) << "Found " << coordinate.x << ", " << coordinate.y;
  //     LOG(LOG_DEBUG) << "Before removal. Size" << getSize();
  //     it = m_zoneNodes.erase(it);
  //     LOG(LOG_DEBUG) << "After  removal. Size" << getSize();
  //   }
  //   else
  //   {
  //     it++;
  //   }
  // }
}

void ZoneArea::occupyZoneNode(Point coordinate)
{
  for (auto &zoneNode : m_zoneNodes)
  {
    zoneNode.occupied = true;
  }
}

void ZoneArea::freeZoneNode(Point coordinate)
{
  for (auto &zoneNode : m_zoneNodes)
  {
    zoneNode.occupied = false;
  }
}
