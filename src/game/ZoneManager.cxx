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
  constexpr int amountOfBuildingsToSpawn = 5;
  auto &randomizer = Randomizer::instance();
  // shuffle mapNodes so placement of building looks random
  randomizer.shuffle(m_MapNodes.begin(), m_MapNodes.end());

  int buildingsSpawned = 0;
  // pick every single zone node we have
  for (auto &node : m_MapNodes)
  {
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }
    // if a building is bigger than one node, the adjacent nodes will no longer be a zone node and buildings can't be spawned there
    // consider that we're in a loop and if the zones stay, they will still be in the vector we're iterating over.
    if (!node || !node->getTileData(Layer::ZONE))
    {
      continue; // if the node that is still in the vector is no longer a zone node, skip
    }

    // a building can be tied to multiple zones. So get all elligible zones for this building
    std::vector<std::string> availableZoneTiles;

    Zones thisZone = Zones::RESIDENTIAL; // initialize zone with residential. Maybe add invalid later?
    // a zone tile only has one zone tied to it. the one it represents, so pick first element of vector
    if (node->getTileData(Layer::ZONE)->zones[0])
    {
      thisZone = node->getTileData(Layer::ZONE)->zones[0];
    }

    // get the maximum size we can spawn at this node, but limit it by 4x4 tiles
    unsigned int maxSizeX = std::min(4, static_cast<int>(getMaximumTileSize(node->getCoordinates()).width));
    unsigned int maxSizeY = std::min(4, static_cast<int>(getMaximumTileSize(node->getCoordinates()).height));
    TileSize maxTileSize = {maxSizeX, maxSizeY};
    std::string building = TileManager::instance().getRandomTileIDForZoneWithRandomSize(thisZone, maxTileSize);

    // place the building
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), building);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), building, false);
    buildingsSpawned++;
  }
}

void ZoneManager::addZoneNode(MapNode *node) { m_MapNodes.emplace_back(node); }
void ZoneManager::removeZoneNode(MapNode *node)
{
  if (node)
  {
    m_MapNodes.erase(std::remove(m_MapNodes.begin(), m_MapNodes.end(), node));
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

TileSize ZoneManager::getMaximumTileSize(Point originPoint)
{
  TileSize possibleSize;

  for (int distance = 1; distance <= possibleSize.width || distance <= possibleSize.height; distance++)
  {
    const MapNode *currentNodeInXDirection = getZoneNodeWithCoordinate({originPoint.x - distance, originPoint.y});
    const MapNode *currentNodeInYDirection = getZoneNodeWithCoordinate({originPoint.x, originPoint.y + distance});

    if (currentNodeInXDirection && currentNodeInXDirection->getTileData(Layer::ZONE))
    {
      possibleSize.width++;
    }
    if (currentNodeInYDirection && currentNodeInYDirection->getTileData(Layer::ZONE))
    {
      possibleSize.height++;
    }
  }
  return possibleSize;
}

const MapNode *ZoneManager::getZoneNodeWithCoordinate(Point coordinate)
{
  for (const MapNode *node : m_MapNodes)
  {
    if (node->getCoordinates().x == coordinate.x && node->getCoordinates().y == coordinate.y)
    {
      return node;
    }
  }
  return nullptr;
}