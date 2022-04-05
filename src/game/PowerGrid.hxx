#ifndef POWERGRID_HXX_
#define POWERGRID_HXX_

#include "point.hxx"
#include <vector>

struct PowerPlant
{
  Point coordinate;
  int powerProduction = 0;
};

class PowerGrid
{
public:
  PowerGrid();

private:
  std::vector<PowerPlant> m_powerPlants;
  std::vector<Point> doesnotwork;
};

#endif