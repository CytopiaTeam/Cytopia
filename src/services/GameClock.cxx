#include "GameClock.hxx"

GameClock::GameClock(GameService::ServiceTuple &services) : GameService(services) {}

void GameClock::tickRealTime()
{
  auto now = Clock::now();

  while (!m_realTimeTasks.isEmpty() && (now > m_realTimeTasks.top().m_waketime))
  {
    const auto task = m_realTimeTasks.top();
    m_realTimeTasks.pop();
    const bool isCanceled = task.callback();

    if (!isCanceled && (task.m_period != 0U))
    {
      m_realTimeTasks.add(
          RealTimeClockTask(task.callback, TimePoint(Clock::now() + (task.m_period - TimePointZero)), task.m_period, task.hndl));
    }
  }
}

void GameClock::tick(void)
{
  std::lock_guard<std::mutex> lock(m_lock);

  // Todo: rework to fix code duplication
  tickRealTime();

  auto now = Clock::now();
  if ((now - m_lastGameTickTime) >= m_gameTickDuration)
  {
    m_lastGameTickTime = now;
    m_gameTicks++;

    while (!m_gameTimeTasks.isEmpty() && (m_gameTicks >= m_gameTimeTasks.top().m_waketime))
    {
      const auto task = m_gameTimeTasks.top();
      m_gameTimeTasks.pop();
      const bool isCanceled = task.callback();

      if (!isCanceled && (task.m_period != 0U))
      {
        m_gameTimeTasks.add(GameTimeClockTask(task.callback, m_gameTicks + task.m_period, task.m_period, task.hndl));
      }
    }
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
