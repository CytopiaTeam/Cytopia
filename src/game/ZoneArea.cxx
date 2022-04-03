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
    if (node.occupied)
    {
      continue;
    }

    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }
    // get the maximum size we can spawn at this node
    TileSize maxTileSize = {getMaximumTileSize(node.coordinate).width, getMaximumTileSize(node.coordinate).height};
    std::string buildingTileID = TileManager::instance().getRandomTileIDForZoneWithRandomSize(getZone(), getZoneDensity(), maxTileSize);

    // place the building
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node.coordinate, buildingTileID);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), buildingTileID, false);
    buildingsSpawned++;
  }
}

bool ZoneArea::isPartOfZone(Point coordinate) const
{
  return m_zoneNodes.end() != std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(),
                                           [&coordinate](const ZoneNode &node) { return node.coordinate == coordinate; });
}

bool ZoneArea::isVacant() const
{
  return m_zoneNodes.end() !=
         std::find_if(m_zoneNodes.begin(), m_zoneNodes.end(), [](const ZoneNode &node) { return node.occupied == false; });
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
  m_zoneNodes.erase(std::remove_if(m_zoneNodes.begin(), m_zoneNodes.end(),
                                   [coordinate](const ZoneNode &zone) { return zone.coordinate == coordinate; }),
                    m_zoneNodes.end());
}

void ZoneArea::occupyZoneNode(Point coordinate)
{
  for (auto &zoneNode : m_zoneNodes)
  {
    if (zoneNode.coordinate == coordinate)
    {
      zoneNode.occupied = true;
    }
  }
}

void ZoneArea::freeZoneNode(Point coordinate)
{
  for (auto &zoneNode : m_zoneNodes)
  {
    if (zoneNode.coordinate == coordinate)
    {
      zoneNode.occupied = false;
    }
  }
}
