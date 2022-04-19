#ifndef POWERGRID_HXX_
#define POWERGRID_HXX_

#include "point.hxx"
#include <vector>

struct PowerNode
{
  Point coordinate;
  int powerProduction = 0;
};

class PowerGrid;
void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged);
class PowerGrid
{
public:
  PowerGrid(PowerNode powerNode);

  /**
   * @brief Add a powernode to this powergrid
   * 
   * @param zoneNode to add
   */
  void addPowerNode(PowerNode powerNode);

  /**
   * @brief Remove a powernode on a given coordinate from this powerGrid
   * 
   * @param Coordinates of the mapNode with a power tile
   */
  void removePowerNode(Point coordinate);

  /**
   * @brief If this coordinate is a neighbor of one of the tiles in this power grid.
   * 
   * @param coordinate The point to check
   * @return neighbor of this power grid
   */
  bool isNeighbor(Point coordinate) const;

  /**
   * @brief If this coordinate part of this power grid.
   * 
   * @param coordinate The point to check
   * @return neighbor of this power grid
   */
  bool isMemberOf(Point coordinate) const;

  auto begin() { return m_powerNodes.begin(); }
  auto end() { return m_powerNodes.end(); }

private:
  //TODO: figure out how to handle multi node power plants
  std::vector<PowerNode> m_powerNodes;
  friend void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged);

  int m_powerLevel = 0;
};

#endif