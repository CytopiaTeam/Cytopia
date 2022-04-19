#ifndef POWERMANAGER_HXX_
#define POWERMANAGER_HXX_

#include "PowerGrid.hxx"

class PowerManager
{
public:
  PowerManager();

private:
  int m_powerProduced = 0;

  void update();
  std::vector<int> getAdjacentPowerGrids(const PowerNode &powerNode, std::vector<PowerGrid> &powerGrids);
  void addPowerNodeToGrid(PowerNode &powerNode, std::vector<PowerGrid> &powerGrids);

  std::vector<PowerNode> m_nodesToAdd;
  std::vector<PowerGrid> m_powerGrids;
};

#endif