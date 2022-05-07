#include "PowerGrid.hxx"
#include "Engine.hxx"
#include "LOG.hxx"

PowerGrid::PowerGrid(PowerNode powerNode) : m_powerNodes{powerNode} {}

void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged)
{
  mainGrid.m_powerNodes.insert(mainGrid.m_powerNodes.end(), toBeMerged.m_powerNodes.begin(), toBeMerged.m_powerNodes.end());
}

void PowerGrid::addPowerNode(PowerNode powerNode)
{
  m_powerNodes.push_back(powerNode);
}

void PowerGrid::updatePowerLevel()
{
  m_powerLevel = 0; // reset the powerlevels
  for (const auto &node : m_powerNodes)
  {
    // For multi-tile buildings, each tile has the same tiledata (including power level).
    // so make sure we only add it once (if it's the origin node)
    if (Engine::instance().map->getMapNode(node.coordinate).isOriginNode())
    {
      m_powerLevel += node.powerProduction;
        // LOG(LOG_INFO) << "Adding power level: " << node.powerProduction << "/" << m_powerLevel;

      // if (Engine::instance().map->getMapNode(node.coordinate).getTileData(Layer::BUILDINGS) && node.powerProduction == 0)
      // {
      //   int powerLevel = -10;
      //   LOG(LOG_INFO) << "Assumed power level: " << powerLevel << "/" << m_powerLevel;

      //   // m_powerLevel += powerLevel;
      // }
    }
  }
  LOG(LOG_INFO) << "Done calculating power level: " << m_powerLevel;
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