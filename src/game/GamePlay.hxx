#ifndef GAMEPLAY_HXX_
#define GAMEPLAY_HXX_

#include "ZoneManager.hxx"
#include "PowerManager.hxx"
#include "Singleton.hxx"

class GamePlay
{
public:
  GamePlay() = default;

  void resetManagers();
  ZoneManager &getZoneManager() { return m_ZoneManager; };

private:
  ZoneManager m_ZoneManager;
  PowerManager m_PowerManager;
};

#endif