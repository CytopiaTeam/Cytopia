#include "PowerManager.hxx"
#include "../engine/Engine.hxx"
#include "../services/GameClock.hxx"

PowerManager::PowerManager()
{
    Engine::instance().map->registerCbPlacePowerBuilding(
      [this](const MapNode &mapNode) { // If we place a zone tile, add it to the cache to update next tick
        PowerNode nodeToAdd = {mapNode.getCoordinates(), mapNode.getTileData(Layer::BUILDINGS)->power};
        m_nodesToAdd.push_back(nodeToAdd);
      });

  GameClock::instance().addRealTimeClockTask(
      [this]()
      {
        update();
        return false;
      },
      1s, 1s);
}

void PowerManager::update()
{
    if (!m_nodesToAdd.empty())
  {
    for (auto nodeToAdd : m_nodesToAdd)
    {
      LOG(LOG_INFO) << "Adding a node!";
      // addZoneNodeToArea(nodeToAdd, m_zoneAreas);
    }
    m_nodesToAdd.clear();
  }
  // do stuff like update power production, update powerlines, ...
}