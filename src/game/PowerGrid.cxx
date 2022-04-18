#include "PowerGrid.hxx"

void PowerGrid::addPowerNode(PowerNode powerNode)
{
  m_powerNodes.push_back(powerNode);
}

void PowerGrid::removePowerNode(Point coordinate)
{
  m_powerNodes.erase(std::remove_if(m_powerNodes.begin(), m_powerNodes.end(),
                                   [coordinate](const PowerNode &powerNode) { return powerNode.coordinate == coordinate; }),
                    m_powerNodes.end());
}