#include "PowerGrid.hxx"
#include "Engine.hxx"
#include "LOG.hxx"

PowerGrid::PowerGrid(PowerNode powerNode) : m_powerNodes{powerNode} {}

void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged)
{
  mainGrid.m_powerNodes.insert(mainGrid.m_powerNodes.end(), toBeMerged.m_powerNodes.begin(), toBeMerged.m_powerNodes.end());
  mainGrid.m_powerLevel = toBeMerged.m_powerLevel;
};

void PowerGrid::addPowerNode(PowerNode powerNode)
{
  m_powerNodes.push_back(powerNode);
  updatePowerLevel();
}

void PowerGrid::updatePowerLevel()
{
  m_powerLevel = 0; // reset the powerlevels
  for (auto node : m_powerNodes)
  {
    // For multi-tile buildings, each tile has the same tiledata (including power level). 
    // so make sure we only add it once (if it's the origin node)
    if (Engine::instance().map->getMapNode(node.coordinate).isOriginNode())
    {
      m_powerLevel += node.powerProduction;
    }
  }
}

void PowerGrid::removePowerNode(Point coordinate)
{
  m_powerNodes.erase(std::remove_if(m_powerNodes.begin(), m_powerNodes.end(),
                                    [coordinate](const PowerNode &powerNode) { return powerNode.coordinate == coordinate; }),
                     m_powerNodes.end());
}

bool PowerGrid::isMemberOf(Point coordinate) const
{
  return m_powerNodes.end() != std::find_if(m_powerNodes.begin(), m_powerNodes.end(),
                                            [&coordinate](const PowerNode &node) { return node.coordinate == coordinate; });
}

bool PowerGrid::isNeighbor(Point coordinate) const
{
  for (const PowerNode &node : m_powerNodes)
  {
    if (node.coordinate.isDirectNeighborOf(coordinate))
    {
      return true;
    }
  }

  return false;
}