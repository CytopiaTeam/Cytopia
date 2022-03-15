#include "GamePlay.hxx"

void GamePlay::tick()
{
  initZoneTimer();
}

void GamePlay::initZoneTimer()
{

  //TODO: Use a gametime task here once the values are adjusted
  m_GameClock.addRealTimeClockTask(
  [this]()
  {
    m_ZoneManager.update();
    return false;
  },
  1s, 1s);
}