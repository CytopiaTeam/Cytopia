#include "GameClock.hxx"

template <typename Task, typename Cmp, typename Now> void GameClock::tickTask(PriorityQueue<Task, Cmp> &queue, const Now now)
{
  while (!queue.empty() && (now >= queue.top().m_waketime))
  {
    const auto task = queue.top();
    queue.pop();
    const bool isCanceled = task.callback();

    if (!isCanceled && (task.m_period != decltype(task.m_period){0}))
    {
      queue.push(Task(task.callback, now + task.m_period, task.m_period, task.hndl));
    }
  }
}

void GameClock::tick(void)
{
  std::lock_guard<std::mutex> lock(m_lock);

  const auto now = Clock::now();

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
  return queue.erase_if([hndl](const typename Queue::value_type &ct) { return ct.hndl == hndl; }) > 0;
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
  assert(cbk != nullptr);

  std::lock_guard<std::mutex> lock(m_lock);

  // If no delay required call callback immediately
  if (delay == 0)
  {
    const bool isCanceled = cbk();

    if (isCanceled || (period == 0))
    {
      return ClockTaskHndlInvalid;
    }

    delay = period;
  }

  // Add +1 tick just to be sure timer is not fire before its timeout.
  m_gameTimeTasks.push(GameTimeClockTask(cbk, delay + m_gameTicks + 1U, period, ++m_unique_handle));
  return m_unique_handle;
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
