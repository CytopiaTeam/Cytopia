#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"

// Engine::instance().map-

void ZoneManager::updateZones()
{
  LOG(LOG_INFO) << "Updating zones" << m_MapNodes.size();
  m_MapNodes.clear();
  for(auto &node : Engine::instance().map->mapNodes)
  {
    if (node.getTileData(ZONE)) // if there's a zone this layer is not null
    {
      LOG(LOG_INFO) << "Adding zones for " << node.getTileID(ZONE);
      m_MapNodes.emplace_back(&node);
    }
  }
}

void ZoneManager::spawnBuildings()
{
  for(auto node : m_MapNodes)
  {
  LOG(LOG_INFO) << "Spawning buildings in zones";

    std::string tileToPlace = "res_1x1_scandinavian_house2_Y8";
     std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), tileToPlace);
     Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), tileToPlace, false);

    //  node->setTileID("res_1x1_scandinavian_house2_Y8", node->getCoordinates());
     
  }
}