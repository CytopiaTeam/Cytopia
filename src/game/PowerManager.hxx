#ifndef POWERMANAGER_HXX_
#define POWERMANAGER_HXX_

#include "PowerGrid.hxx"
#include <MapNode.hxx>

class PowerManager
{
public:
  PowerManager();

  void reset();

private:
  int m_powerProduced = 0;

  void update();
  std::vector<int> getAdjacentPowerGrids(const PowerNode &powerNode, std::vector<PowerGrid> &powerGrids);
  void addPowerNodeToGrid(PowerNode &powerNode, std::vector<PowerGrid> &powerGrids);
  void removePowerNode(Point coordinate);
  std::vector<PowerGrid> rebuildZoneArea(PowerGrid &powerGrid);

  void updateRemovedNodes(const MapNode *mapNode);
  void updatePlacedNodes(const MapNode &mapNode);

  /**
   * @brief Updates all PowerGrid's power levels
   * @details Iterates through m_powerGrids and calls updatePowerLevel on each grid.
   * @see PowerGrid#updatePowerLevel
   */
  void updatePowerLevels();

  std::vector<PowerNode> m_nodesToAdd;
  std::vector<Point> m_nodesToRemove;
  std::vector<PowerGrid> m_powerGrids;
};

#endif