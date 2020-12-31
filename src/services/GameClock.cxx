#include "GameClock.hxx"

GameClock::GameClock(GameService::ServiceTuple &services) : GameService(services) {}

void GameClock::tick(void)
{
  std::lock_guard<std::mutex> lock(m_lock);

  auto now = Clock::now();

  tickTask(m_realTimeTasks, now);

  if ((now - m_lastGameTickTime) >= m_gameTickDuration)
  {
    m_lastGameTickTime = now;
    m_gameTicks++;

    tickTask(m_gameTimeTasks, m_gameTicks);
  }
}

template <typename Queue> static bool removeTaskFromQueue(const GameClock::ClockTaskHndl hndl, Queue &queue)
{
  auto position = std::find_if(queue.begin(), queue.end(), [hndl](auto it) { return it.hndl == hndl; });

  if (position != queue.end())
  {
    queue.remove(position);
    return true;
  }

  return false;
}

bool GameClock::removeClockTask(ClockTaskHndl hndl)
{
  std::lock_guard<std::mutex> lock(m_lock);

  if (!removeTaskFromQueue(hndl, m_realTimeTasks))
  {
    return removeTaskFromQueue(hndl, m_gameTimeTasks);
  }

  return true;
}

GameClock::ClockTaskHndl GameClock::addGameTimeClockTask(ClockCbk cbk, GameClockTime delay, GameClockTime period)
{
  if ((cbk != nullptr) && (delay > 0U))
  {
    std::lock_guard<std::mutex> lock(m_lock);
    // Add +1 tick just to be sure timer is not fire before its timeout.
    m_gameTimeTasks.add(GameTimeClockTask(cbk, delay + m_gameTicks + 1U, period, ++m_unique_handle));
    return m_unique_handle;
  }
  else
  {
    return ClockTaskHndlInvalid;
  }
}

void GameClock::setGameClockSpeed(float speedFactor)
{
  std::lock_guard<std::mutex> lock(m_lock);
  m_gameTickDuration = std::chrono::milliseconds((unsigned int)(GameClock::DefaultGameTickDuration / speedFactor));
}

void GameClock::clear(void)
{
  std::lock_guard<std::mutex> lock(m_lock);
  m_gameTimeTasks.clear();
  m_realTimeTasks.clear();
}
