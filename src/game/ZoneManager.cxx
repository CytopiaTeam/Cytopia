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

  for (auto node : m_MapNodes)
  {
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }

    // get all tiles from Category
    std::vector<std::string> myVec = TileManager::instance().getTileIDsOfCategory(Zones::RESIDENTIAL);
    int randomIndex = rand() % myVec.size();

    std::string tileToPlace = myVec[randomIndex];
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), tileToPlace);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), tileToPlace, false);

    //  node->setTileID("res_1x1_scandinavian_house2_Y8", node->getCoordinates());
    buildingsSpawned++;
  }
}