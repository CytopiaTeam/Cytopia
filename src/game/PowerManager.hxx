#ifndef POWERMANAGER_HXX_
#define POWERMANAGER_HXX_

#include "PowerGrid.hxx"

class PowerManager
{
public:
  PowerManager();

private:
  int m_powerProduced = 0;

  void tick();
};

#endif