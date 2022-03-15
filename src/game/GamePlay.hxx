#ifndef GAMEPLAY_HXX_
#define GAMEPLAY_HXX_

#include "ZoneManager.hxx"

class GamePlay
{
public:
  GamePlay() = default;

  void update();

private:
  ZoneManager m_ZoneManager;
};

#endif