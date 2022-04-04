#include "PowerManager.hxx"
#include "../services/GameClock.hxx"

PowerManager::PowerManager()
{
  GameClock::instance().addRealTimeClockTask(
      [this]()
      {
        tick();
        return false;
      },
      1s, 1s);
}

void PowerManager::tick()
{
  // do stuff like update power production, update powerlines, ...
}