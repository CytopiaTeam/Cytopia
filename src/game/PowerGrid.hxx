#ifndef POWERGRID_HXX_
#define POWERGRID_HXX_

#include "Point.hxx"
#include "../engine/GameObjects/MapGrid.hxx"
#include <vector>

struct PowerNode
{
  Point coordinate;
  int powerProduction = 0;
};

class PowerGrid;
void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged);

class PowerGrid : public MapGrid<PowerNode>
{
public:
  PowerGrid(PowerNode powerNode);

  /**
   * @brief Get the Power Level of this grid
   * 
   * @return int power level in MW
   */
  int getPowerLevel() const { return m_powerLevel; };

  /**
   * @brief Recalculate the power level of this power grid
   * 
   */
  void updatePowerLevel();

private:
  friend void mergePowerGrids(PowerGrid &mainGrid, PowerGrid &toBeMerged);

  int m_powerLevel = 0;
};

#endif
