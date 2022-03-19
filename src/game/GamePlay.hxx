#ifndef GAMEPLAY_HXX_
#define GAMEPLAY_HXX_

#include "ZoneManager.hxx"
#include "Singleton.hxx"

class GamePlay : public Singleton<GamePlay>
{
public:
  GamePlay() = default;

  void update();
  ZoneManager *getZoneManager() { return &m_ZoneManager; };

private:
  ZoneManager m_ZoneManager;
};

#endif