#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"
#include "../util/RandomEngine.hxx"
#include "../services/GameClock.hxx"

#include <algorithm>
#include <random>

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
  updateZones();
  spawn();
}

void ZoneManager::updateZones()
{
  m_MapNodes.clear();
  for (const MapNode &node : Engine::instance().map->getMapNodes())
  {
    if (node.getTileData(ZONE)) // if there's a zone this layer is not null
    {
      m_MapNodes.emplace_back(&node);
    }
  }
}

void ZoneManager::spawn()
{
  constexpr int amountOfBuildingsToSpawn = 5;
  int buildingsSpawned = 0;

  // shuffle mapNodes
  std::shuffle(std::begin(m_MapNodes), std::end(m_MapNodes), RandomEngine::instance().getRandomEngine());

  // pick every single zone node we have
  for (auto& node : m_MapNodes)
  {
    std::vector<std::string> myVec;
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }

    if (!node->getTileData(Layer::ZONE))
    {
      LOG(LOG_ERROR) << "Something is wrong with a zone tile - Report this to the Cytopia Team!";
      continue;
    }
    // get the right zone enum of the zone tile
    for (auto& zone : node->getTileData(Layer::ZONE)->zones)
    {
      if (TileManager::instance().getTileIDsOfCategory(zone).empty())
      {
        LOG(LOG_ERROR) << "No buildings available for zone: " << zone;
        continue;
      }
      else
      {
        // add all the tileIDs that are available for this zone to our vector
        for (const auto &ID : TileManager::instance().getTileIDsOfCategory(zone))
        {
          myVec.emplace_back(ID);
        }
      }
    }

    if (myVec.empty())
    {
      LOG(LOG_ERROR) << "No buildings available! Aborting!";
      break;
    }
    // pick a random tileID from the vector
    // TODO: Check how many neighboring tiles are available and pick a building with correct tilesize
    int randomIndex = rand() % myVec.size();

    std::string building = myVec[randomIndex];
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), building);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), building, false);

    buildingsSpawned++;
  }
}