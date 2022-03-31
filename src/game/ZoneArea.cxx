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
      xmax(std::min(Settings::instance().mapSize, zoneNode.coordinate.x + 1)), ymin(std::max(0, zoneNode.coordinate.y - 1)),
      ymax(std::min(Settings::instance().mapSize, zoneNode.coordinate.y + 1))
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

      std::string building = TileManager::instance().getRandomTileIDForZoneWithRandomSize(getZone(), maxTileSize);

      // place the building
      std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node.coordinate, building);
      Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), building, false);
      buildingsSpawned++;
  }}

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