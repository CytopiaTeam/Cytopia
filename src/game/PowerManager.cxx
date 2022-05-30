#include "PowerManager.hxx"
#include "../services/GameClock.hxx"
#include <MapFunctions.hxx>
#include "GameStates.hxx"
#include <SignalMediator.hxx>

PowerManager::PowerManager()
{
  SignalMediator::instance().registerCbSetTileID(Signal::slot(this, &PowerManager::updatePlacedNodes));
  SignalMediator::instance().registerCbDemolish(Signal::slot(this, &PowerManager::updateRemovedNodes));

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
  bool updated = false;
  // Remove nodes (Demolish on power tiles)
  if (!m_nodesToRemove.empty())
  {
    for (auto m_nodeToRemove : m_nodesToRemove)
    {
      removePowerNode(m_nodeToRemove);
    }

    m_nodesToRemove.clear();
    updated = true;
  }

  if (!m_nodesToAdd.empty())
  {
    for (auto nodeToAdd : m_nodesToAdd)
    {
      addPowerNodeToGrid(nodeToAdd, m_powerGrids);
    }
    m_nodesToAdd.clear();

    updated = true;
  }

  if (updated)
  {
    updatePowerLevels();
    SignalMediator::instance().signalUpdatePower.emit(m_powerGrids);
  }
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
    powerGrids[gridNeighbour[0]].addNode(powerNode);
  }
  else
  { // merge zone areas
    PowerGrid &mergedGrid = powerGrids[gridNeighbour[0]];
    mergedGrid.addNode(powerNode);

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

void PowerManager::removePowerNode(Point coordinate)
{
  for (auto gridIt = m_powerGrids.begin(); gridIt != m_powerGrids.end(); gridIt++)
  {
    if (gridIt->isMemberOf(coordinate))
    {
      gridIt->removeNode(coordinate);

      if (gridIt->size() == 0)
      {
        m_powerGrids.erase(gridIt);
      }
      else
      {
        auto powerGrids = rebuildZoneArea(*gridIt);
        assert(powerGrids.size() > 0);
        // If powerGrids size is 1, means powerGrid is still compact, nothing to be done

        if (powerGrids.size() > 1)
        {
          m_powerGrids.erase(gridIt);
          m_powerGrids.insert(m_powerGrids.end(), powerGrids.begin(), powerGrids.end());
        }
      }

      break;
    }
  }
}

std::vector<PowerGrid> PowerManager::rebuildZoneArea(PowerGrid &powerGrid)
{
  std::vector<PowerGrid> newPowerGrids;

  for (PowerNode powerNode : powerGrid)
  {
    addPowerNodeToGrid(powerNode, newPowerGrids);
  }

  return newPowerGrids;
}
void PowerManager::updatePlacedNodes(const MapNode &mapNode)
{
  if (!mapNode.isConductive())
  {
    return;
  }

  int powerLevelOfTile = 0;
  if (mapNode.getTileData(Layer::BUILDINGS))
  { // it's safe to assume only nodes on BUILDINGS layer produce power
    powerLevelOfTile = mapNode.getTileData(Layer::BUILDINGS)->power;
  }

  PowerNode nodeToAdd = {mapNode.getCoordinates(), powerLevelOfTile};
  m_nodesToAdd.push_back(nodeToAdd);
}

void PowerManager::updateRemovedNodes(const MapNode *mapNode)
{
  switch (GameStates::instance().demolishMode)
  {
  case DemolishMode::DEFAULT:
  {
    if (!mapNode->isConductive())
    { // if there's no power conductor on this node, remove it from the grid
      m_nodesToRemove.push_back(mapNode->getCoordinates());
    }
    break;
  }
  default:
    break;
  }
}

void PowerManager::updatePowerLevels()
{
  for (auto &powerGrid : m_powerGrids)
  {
    powerGrid.updatePowerLevel();
  }
}

void PowerManager::reset()
{
  m_powerGrids.clear();
  for (const auto &mapNode : MapFunctions::instance().getMapNodes())
  {
    if (mapNode.isConductive())
    {
      int power = 0;
      if (mapNode.getTileData(Layer::BUILDINGS))
      {
        power = mapNode.getTileData(Layer::BUILDINGS)->power;
      }
      PowerNode powerNode = {mapNode.getCoordinates(), power};
      m_nodesToAdd.push_back(powerNode);
    }
  }
}
