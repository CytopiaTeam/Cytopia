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
  // LOG(LOG_INFO) << "calling updatePowerLevel" << m_gridNodes.size();
  for (const auto &node : m_gridNodes)
  {
    MapNode &mapNode = MapFunctions::instance().getMapNode(node.coordinate);
    if (!mapNode.isOriginNode())
    { // For multi-tile buildings, each tile has the same tiledata (including power level).
      // so make sure we only add it once (if it's the origin node)
      continue;
    }
    if (mapNode.getTileData(Layer::BUILDINGS) && node.powerProduction == 0)
    { // each occupied node consumes one power unit
      // LOG(LOG_ERROR) << "i make --";
      m_powerLevel--;
    }
    else
    {
      LOG(LOG_ERROR) << "i add " << node.powerProduction;
      m_powerLevel += node.powerProduction;
    }
  }
}
