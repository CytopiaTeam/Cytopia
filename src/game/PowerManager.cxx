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
      addPowerNodeToGrid(nodeToAdd, m_powerGrids);
    }
    m_nodesToAdd.clear();
    // LOG(LOG_DEBUG) << "Number of power areas: " << m_powerGrids.size();
  }
  // do stuff like update power production, update powerlines, ...
}

std::vector<int> PowerManager::getAdjacentPowerGrids(const PowerNode &powerNode, std::vector<PowerGrid> &powerGrids)
{
  std::vector<int> neighborGrids;
  int i = 0;

  for (auto &powerGrid : powerGrids)
  {
    if (powerGrid.isNeighbor(powerNode.coordinate))
    {
      neighborGrids.push_back(i);
    }
    ++i;
  }

  return neighborGrids;
}

void PowerManager::addPowerNodeToGrid(PowerNode &powerNode, std::vector<PowerGrid> &powerGrids)
{
  auto gridNeighbour = getAdjacentPowerGrids(powerNode, powerGrids);

  if (gridNeighbour.empty())
  { // new powergrid
    powerGrids.emplace_back(powerNode);
  }
  else if (gridNeighbour.size() == 1)
  { // add to this grid
    powerGrids[gridNeighbour[0]].addPowerNode(powerNode);
  }
  else
  { // merge zone areas
    PowerGrid &mergedGrid = powerGrids[gridNeighbour[0]];
    mergedGrid.addPowerNode(powerNode);

    for (int idx = 1; idx < gridNeighbour.size(); ++idx)
    {
      mergePowerGrids(mergedGrid, powerGrids[gridNeighbour[idx]]);
    }

    for (int idx = gridNeighbour.size() - 1; idx > 0; --idx)
    {
      powerGrids.erase(powerGrids.begin() + gridNeighbour[idx]);
    }
  }
}
