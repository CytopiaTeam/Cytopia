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

  size_t size() { return m_powerNodes.size(); };

  /**
   * @brief Add a powernode to this powergrid
   * 
   * @param powerNode node to add
   */
  void addPowerNode(PowerNode powerNode);

  /**
   * @brief Remove a powernode on a given coordinate from this powerGrid
   * 
   * @param coordinate Coordinates of the mapNode with a power tile
   */
  void removePowerNode(Point coordinate);

  /**
   * @brief If this coordinate is a neighbor of one of the tiles in this power grid.
   * 
   * @param coordinate The point to check
   * @return if the given coordinate is a neighbor of this power grid
   */
  bool isNeighbor(Point coordinate) const;

  /**
   * @brief If this coordinate is part of this power grid.
   * 
   * @param coordinate The point to check
   * @return if the given coordinate is a member of this power grid.
   */
  bool isMemberOf(Point coordinate) const;

  /**
   * @brief Get the Power Level of this grid
   * 
   * @return int power level in MW
   */
  int getPowerLevel() const { return m_powerLevel; };

  /// iterators
  auto begin() { return m_powerNodes.begin(); }
  auto end() { return m_powerNodes.end(); }

private:
  //TODO: figure out how to handle multi node power plants
  std::vector<PowerNode> m_powerNodes;
  friend void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged);

  void updatePowerLevel();
  int m_powerLevel = 0;
};

#endif