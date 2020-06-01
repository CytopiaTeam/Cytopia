#include "GameClock.hxx"

GameClock::GameClock(GameService::ServiceTuple &services) : GameService(services), m_lock(), m_lastGameTickTime(Clock::now()) {}

void GameClock::tick(void)
{
  auto now = Clock::now();
  std::lock_guard<std::mutex> lock(m_lock);

  while (!m_realTimeTasks.empty() && (now > m_realTimeTasks.top().m_waketime))
  {
    const auto task = m_realTimeTasks.top();
    m_realTimeTasks.pop();
    task.callback();

    if (task.m_period != TimePointZero)
    {
      m_realTimeTasks.emplace(RealTimeClockTask(task.callback,
                                                TimePoint(Clock::now() + (TimePoint(task.m_period) - TimePointZero)),
                                                TimePoint(task.m_period), task.hndl));
    }
  }

  if ((now - m_lastGameTickTime) >= m_gameTickDuration)
  {
    m_lastGameTickTime = now;
    m_gameTicks++;

    while (!m_gameTimeTasks.empty() && (m_gameTicks >= m_gameTimeTasks.top().m_waketime))
    {
      const auto task = m_gameTimeTasks.top();
      m_gameTimeTasks.pop();
      task.callback();

      if (task.m_period != 0U)
      {
        m_gameTimeTasks.emplace(GameTimeClockTask(task.callback, m_gameTicks + task.m_period, task.m_period, task.hndl));
      }
    }
  }
}

bool GameClock::removeClockTask(ClockTaskHndl hndl)
{
  std::lock_guard<std::mutex> lock(m_lock);

  if (m_realTimeTasks.remove(hndl))
  {
    return true;
  }
  else
  {
    return m_gameTimeTasks.remove(hndl);
  }
}

GameClock::ClockTaskHndl GameClock::addGameTimeClockTask(ClockCbk cbk, GameClockTime delay, GameClockTime period)
{
  if ((cbk != nullptr) && (delay > 0U))
  {
    std::lock_guard<std::mutex> lock(m_lock);
    // Add +1 tick just to be sure timer is not fire before its timeout.
    m_gameTimeTasks.emplace(GameTimeClockTask(cbk, delay + m_gameTicks + 1U, period, ++m_unique_handle));
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

template <typename Task> bool GameClock::RemovablePriorityQueue<Task>::remove(GameClock::ClockTaskHndl hndl)
{
  auto task = std::find_if(c.begin(), c.end(), [hndl](auto it) { return it.hndl == hndl; });

  if (task != c.end())
  {
    this->c.erase(task);
    return true;
  }

  return false;
}

template <typename Task> void GameClock::RemovablePriorityQueue<Task>::clear(void) { c.clear(); }