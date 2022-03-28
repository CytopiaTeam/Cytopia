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

void ZoneManager::addZoneNode(Point coordinate, Zones zone, ZoneDensity ZoneDensity)
{
  // NOTE: Ignore density for agricultural zone
  // do
  ZoneNode newZone;
  newZone.coordinate = coordinate;
  newZone.zone = zone;
  newZone.ZoneDensity = ZoneDensity;

  m_AllNodes.push_back(newZone);
  rebuildZoneAreas();
}

void ZoneManager::rebuildZoneAreas()
{
  m_zoneAreas.clear(); // clear the vector, we want to rebuild it

  std::vector<Point> sortedZoneNodes;
  // std::sort(m_AllNodes.begin(), m_AllNodes.end(), compare_xy());
  for (auto zoneNode : m_AllNodes)
  {
    LOG(LOG_INFO) << "unsorted: " << zoneNode.coordinate.x << ", " << zoneNode.coordinate.y;
  }

  std::sort(m_AllNodes.begin(), m_AllNodes.end(),
            [](const auto &x, const auto &y)
            {
    
              // return (x.coordinate.x == y.coordinate.x ? x.coordinate.y < y.coordinate.y : x.coordinate.x < y.coordinate.x);

              int xIdx = (x.coordinate.x - Settings::instance().mapSize) + (x.coordinate.y - Settings::instance().mapSize);
              int yIdx = (y.coordinate.x - Settings::instance().mapSize) + (y.coordinate.y - Settings::instance().mapSize);
              int distanceFromAToB = x.coordinate.manhattanDistanceTo(y.coordinate);
              Point zero = {0, 0};
              int distX = zero.distanceTo(x.coordinate);
              int distY = zero.distanceTo(y.coordinate);
              LOG(LOG_INFO) << "Distance from " << x.coordinate.x << ", " << x.coordinate.y << " to " << y.coordinate.x << ", "
                            << y.coordinate.y << " is: " << x.coordinate.distanceTo(y.coordinate);
              LOG(LOG_INFO) << "X Distance to zero: " << distX;
              LOG(LOG_INFO) << "Y Distance to zero: " << distY;
              return distX < distY; //x.coordinate.y.manhattanDistanceTo(y.coordinate.y);
            });

  for (auto zoneNode : m_AllNodes)
  {
    LOG(LOG_INFO) << "sorted: " << zoneNode.coordinate.x << ", " << zoneNode.coordinate.y;
  }

// debug print coords and distance
  for (auto it : m_AllNodes)
  {
    Point zero = {0, 0};
    std::cout << it.coordinate.x << ", " << it.coordinate.y << "( " << zero.manhattanDistanceTo(it.coordinate) << " ) | ";
  }

  // return;
  // TODO: iterate over nodes - iterate over zoneareas - iterate over all zone nodes within the area and check if the new tile is a neighbor of any existing.
  // pick a node, get it's neighbor coords and look if we have one.
  for (auto zoneNode : m_AllNodes)
  {
    bool foundNodeInArea = false;

    // get all possible neighbors for this node
    for (auto possibleNeighbor : PointFunctions::getNeighbors(zoneNode.coordinate, false))
    {
      // iterate over all zoneAreas and check if a neighbor is already part of a zone
      for (auto zoneArea : m_zoneAreas)
      {
        // LOG(LOG_INFO) << "I check for isPartOf zone";
        if (zoneArea.isPartOfZone(possibleNeighbor))
        {
          // LOG(LOG_INFO) << "It is";

          // if we've found that this node is a neighbor, add it to the zoneArea.
          zoneArea.addZoneNode(zoneNode);
          foundNodeInArea = true;
          continue; // possibly only breaks the second forloop
        }
      }
    }
    if (!foundNodeInArea)
    {
      // LOG(LOG_INFO) << "Didn't find it. add a new one";
      ZoneArea newZoneArea;
      newZoneArea.addZoneNode(zoneNode);
      m_zoneAreas.push_back(newZoneArea);
    }
  }

  // for (auto zoneNode : m_AllNodes)
  // {
  //   bool isNewArea = true;
  //   // check if this node is part of existing area, if so add it.
  //   for (auto zoneArea : m_zoneAreas)
  //   {
  //     if (zoneArea.isPartOfZone(zoneNode.coordinate))
  //     {
  //       // LOG(LOG_INFO) << "it is part";
  //       isNewArea = false;
  //       zoneArea.addZoneNode(zoneNode);
  //     }
  //   }

  //   if (isNewArea && !isFirst)
  //   {
  //     // LOG(LOG_INFO) << "it is new!";
  //     ZoneArea newZoneArea;
  //     // add node to new area
  //     newZoneArea.addZoneNode(zoneNode);
  //     // add new area to vector
  //     m_zoneAreas.push_back(newZoneArea);
  //   }
  //   isFirst = false;
  // }

  LOG(LOG_INFO) << "I have " << m_zoneAreas.size() << "areas!";
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