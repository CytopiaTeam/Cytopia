template <typename DelayType, typename PeriodType>
GameClock::ClockTaskHndl GameClock::addRealTimeClockTask(ClockCbk cbk, DelayType delay, PeriodType period)
{
  if ((cbk != nullptr) && (TimePoint(delay) > TimePointZero) && (TimePoint(period) >= TimePointZero))
  {
    std::lock_guard<std::mutex> lock(m_lock);
    m_realTimeTasks.emplace(RealTimeClockTask(cbk, TimePoint(Clock::now() + (TimePoint(delay) - TimePointZero)),
                                              TimePoint(period), ++m_unique_handle));
    return m_unique_handle;
  }
  else
  {
    return ClockTaskHndlInvalid;
  }
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
