#ifndef GAMEPLAY_HXX_
#define GAMEPLAY_HXX_

#include "ZoneManager.hxx"
#include "../services/GameClock.hxx"

class GamePlay
{
public:
  GamePlay() = default;
  ~GamePlay() = default;
  void tick();

private:
  void initZoneTimer();
  ZoneManager m_ZoneManager;
  GameClock &m_GameClock = GameClock::instance();

};

#endif