#include "PowerManager.hxx"
#include "../services/GameClock.hxx"
#include "GameStates.hxx"
#include <MapNode.hxx>
#include <SignalMediator.hxx>

PowerManager::PowerManager()
{
  SignalMediator::instance().registerCbPlacePowerBuilding(
      [this](const MapNode &mapNode) { // If we place a power tile, add it to the cache to update next tick
        PowerNode nodeToAdd = {mapNode.getCoordinates(), mapNode.getTileData(Layer::BUILDINGS)->power};
        m_nodesToAdd.push_back(nodeToAdd);
      });
  SignalMediator::instance().registerCbDemolish(
      [this](const MapNode *mapNode) { // If we demolish a power tile, add it to the cache to update next tick
        switch (GameStates::instance().demolishMode)
        {
        case DemolishMode::DEFAULT:
        {
          if (!mapNode->getTileData(Layer::BUILDINGS))
          { // if there's no building it has been succesfully demolished
            m_nodesToRemove.push_back(mapNode->getCoordinates());
          }
          break;
        }
        default:
          break;
        }
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
  // Remove nodes (Demolish on power tiles)
  if (!m_nodesToRemove.empty())
  {
    for (auto m_nodeToRemove : m_nodesToRemove)
    {
      removePowerNode(m_nodeToRemove);
    }

    int i = 0;
    for (auto grid : m_powerGrids)
    { // TODO: Remove this later, this is for debugging
      LOG(LOG_DEBUG) << "Grid #" << ++i << "/" << m_powerGrids.size() << " - Power Production: " << grid.getPowerLevel();
    }
    m_nodesToRemove.clear();
  }

  if (!m_nodesToAdd.empty())
  {
    for (auto nodeToAdd : m_nodesToAdd)
    {
      addPowerNodeToGrid(nodeToAdd, m_powerGrids);
    }
    m_nodesToAdd.clear();
    LOG(LOG_DEBUG) << "Number of power areas: " << m_powerGrids.size();
    int i = 0;
    for (auto grid : m_powerGrids)
    { // TODO: Remove this later, this is for debugging
      LOG(LOG_DEBUG) << "Grid #" << ++i << "/" << m_powerGrids.size() << " - Power Production: " << grid.getPowerLevel();
    }
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

void PowerManager::removePowerNode(Point coordinate)
{
  for (auto gridIt = m_powerGrids.begin(); gridIt != m_powerGrids.end(); gridIt++)
  {
    if (gridIt->isMemberOf(coordinate))
    {
      gridIt->removePowerNode(coordinate);

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