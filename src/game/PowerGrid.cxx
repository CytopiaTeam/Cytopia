#include "PowerGrid.hxx"
#include <MapFunctions.hxx>
#include "LOG.hxx"

PowerGrid::PowerGrid(PowerNode powerNode) : MapGrid(powerNode) {}

void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged)
{
  mainGrid.m_gridNodes.insert(mainGrid.end(), toBeMerged.begin(), toBeMerged.end());
}

void PowerGrid::updatePowerLevel()
{
  // reset the power level of this grid before recalculating
  m_powerLevel = 0;
  for (const auto &node : m_gridNodes)
  {
    if (MapFunctions::instance().getMapNode(node.coordinate).getTileData(Layer::BUILDINGS) && node.powerProduction == 0)
    { // each occupied node consumes one power unit
      m_powerLevel--;
    }

    // For multi-tile buildings, each tile has the same tiledata (including power level).
    // so make sure we only add it once (if it's the origin node)
    if (MapFunctions::instance().getMapNode(node.coordinate).isOriginNode())
    {
      m_powerLevel += node.powerProduction;
    }
  }
}
