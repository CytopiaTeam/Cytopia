#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"

#include <algorithm>
#include <random>

// Engine::instance().map-

void ZoneManager::updateZones()
{
  LOG(LOG_INFO) << "Number of Items in zone vector: " << m_MapNodes.size();
  m_MapNodes.clear();
  for (auto &node : Engine::instance().map->mapNodes)
  {
    if (node.getTileData(ZONE)) // if there's a zone this layer is not null
    {
      m_MapNodes.emplace_back(&node);
    }
  }
}

void ZoneManager::spawnBuildings()
{
  int amountOfBuildingsToSpawn = 5;
  int buildingsSpawned = 0;

  // shuffle mapNodes
  auto rd = std::random_device{};
  auto rng = std::default_random_engine{rd()};
  std::shuffle(std::begin(m_MapNodes), std::end(m_MapNodes), rng);

  // pick every single zone node we have
  for (auto node : m_MapNodes)
  {
    std::vector<const std::string> myVec;
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }

    if (!node->getTileData(Layer::ZONE))
    {
      LOG(LOG_ERROR) << "Something is wrong with this tile - Report this to the Cytopia Team!";
      continue;
    }
    // get the right zone enum of the zone zile
    for (auto zone : node->getTileData(Layer::ZONE)->zones)
    {
      if (TileManager::instance().getTileIDsOfCategory(zone).empty())
      {
        LOG(LOG_ERROR) << "No buildings available for zone: " << zone;
        continue;
      }
      else
      {
        // add all the tileIDs that are available for this zone to our vector
        for (auto ID : TileManager::instance().getTileIDsOfCategory(zone))
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

    std::string tileToPlace = myVec[randomIndex];
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), tileToPlace);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), tileToPlace, false);

    buildingsSpawned++;
  }
}