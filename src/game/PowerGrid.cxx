#include "PowerGrid.hxx"

PowerGrid::PowerGrid(PowerNode powerNode) : m_powerNodes{powerNode} {}

void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged)
{
  mainGrid.m_powerNodes.insert(mainGrid.m_powerNodes.end(), toBeMerged.m_powerNodes.begin(), toBeMerged.m_powerNodes.end());
  mainGrid.m_powerLevel = toBeMerged.m_powerLevel;
};

void PowerGrid::addPowerNode(PowerNode powerNode) { m_powerNodes.push_back(powerNode); }

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