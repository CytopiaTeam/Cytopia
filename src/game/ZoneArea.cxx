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
    : m_zoneNodes{zoneNode}, xmin(std::max(0, zoneNode.coordinate.x - 1)), m_zoneType(zoneNode.zoneType),
      m_zoneDensity(zoneNode.zoneDensity), xmax(std::min(Settings::instance().mapSize, zoneNode.coordinate.x + 1)),
      ymin(std::max(0, zoneNode.coordinate.y - 1)), ymax(std::min(Settings::instance().mapSize, zoneNode.coordinate.y + 1))
{
  //update vacancy in case constructor is called with a zonenode
  m_isVacant = checkVacancy();
}

void ZoneArea::spawnBuildings()
{
  constexpr int amountOfBuildingsToSpawn = 5;
  auto &randomizer = Randomizer::instance();
  // shuffle mapNodes so placement of building looks random
  randomizer.shuffle(m_zoneNodes.begin(), m_zoneNodes.end());

  int buildingsSpawned = 0;

  // pick every single zone node we have
  for (auto &node : m_zoneNodes)
  {
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }
    if (node.occupied)
    {
      continue;
    }

    // get the maximum size we can spawn at this node
    TileSize maxTileSize = getMaximumTileSize(node.coordinate);
    std::string buildingTileID =
        TileManager::instance().getRandomTileIDForZoneWithRandomSize(m_zoneType, m_zoneDensity, maxTileSize).value_or("");

    // place the building
    Engine::instance().map->setTileID(buildingTileID, node.coordinate);
    buildingsSpawned++;
  }
}

bool ZoneArea::isWithinZone(Point coordinate) const
{
  return m_zoneNodes.end() != std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),
                                           [&coordinate](const ZoneNode &node) { return node.coordinate == coordinate; });
}

bool ZoneArea::checkVacancy() const
{
  return m_zoneNodes.end() !=
         std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(), [](const ZoneNode &node) { return node.occupied == false; });
}

bool ZoneArea::isNeighborOfZone(Point coordinate) const
{
  for (const ZoneNode &node : m_zoneNodes)
  {
    if(node.coordinate.isDirectNeighborOf(coordinate))
    {
      return true;
    }
  }

  return false;
}

TileSize ZoneArea::getMaximumTileSize(Point originPoint)
{
  TileSize possibleSize = {1, 1};

  for (int distance = 1; distance <= possibleSize.width || distance <= possibleSize.height; distance++)
  {
    std::vector<Point> xDirection = PointFunctions::getArea({originPoint.x - distance, originPoint.y}, originPoint);
    std::vector<Point> yDirection = PointFunctions::getArea(originPoint, {originPoint.x, originPoint.y + distance});
    // check if there's a tile in x direction (top of the origin point)
    bool increaseX = true;
    bool increaseY = true;

    for (auto coord : xDirection)
    {
      if (!isWithinZone(coord))
      {
        increaseX = false;
        break;
      }
    }
    for (auto coord : yDirection)
    {
      if (!isWithinZone(coord))
      {
        increaseY = false;
        break;
      }
    }

    if (increaseX)
    {
      possibleSize.width++;
    }
    if (increaseY)
    {
      possibleSize.height++;
    }
  }
  return possibleSize;
}

void ZoneArea::addZoneNode(ZoneNode zoneNode)
{
  m_zoneNodes.push_back(zoneNode);

  //update vacancy
  m_isVacant = checkVacancy();

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
  m_zoneNodes.erase(std::remove_if(m_zoneNodes.begin(), m_zoneNodes.end(),
                                   [coordinate](const ZoneNode &zone) { return zone.coordinate == coordinate; }),
                    m_zoneNodes.end());
  //update vacancy
  m_isVacant = checkVacancy();
}

void ZoneArea::setVacancy(Point coordinate, bool vacancy)
{
  std::vector<ZoneNode>::iterator node = std::find_if(
      m_zoneNodes.begin(), m_zoneNodes.end(), [coordinate](const ZoneNode &node) { return node.coordinate == coordinate; });
  if (node != m_zoneNodes.end())
  {
    if (vacancy)
    {
      node->occupied = false;
      // std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),
      //            [coordinate](const ZoneNode &node) { return node.coordinate == coordinate; })
      //   ->occupied = false;
    }
    else
    {
      node->occupied = true;

      // std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),
      //              [coordinate](const ZoneNode &node) { return node.coordinate == coordinate; })
      //     ->occupied = true;
    }
    m_isVacant = checkVacancy();
  }
}