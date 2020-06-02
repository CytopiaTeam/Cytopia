template <typename DelayType, typename PeriodType>
GameClock::ClockTaskHndl GameClock::addRealTimeClockTask(ClockCbk cbk, DelayType delay, PeriodType period)
{
  if ((cbk != nullptr) && (TimePoint(delay) > TimePointZero) && (TimePoint(period) >= TimePointZero))
  {
    std::lock_guard<std::mutex> lock(m_lock);
    m_realTimeTasks.add(RealTimeClockTask(cbk, TimePoint(Clock::now() + (TimePoint(delay) - TimePointZero)), TimePoint(period),
                                          ++m_unique_handle));
    return m_unique_handle;
  }
  else
  {
    return ClockTaskHndlInvalid;
  }
}

template <typename Task> void GameClock::RemovablePriorityQueue<Task>::add(Task &&task)
{
  m_container.emplace_back(task);
  std::push_heap(m_container.begin(), m_container.end(), std::greater<Task>{});
}

template <typename Task> void GameClock::RemovablePriorityQueue<Task>::pop(void)
{
  std::pop_heap(m_container.begin(), m_container.end(), std::greater<Task>{});
  m_container.pop_back();
}

template <typename Task> bool GameClock::RemovablePriorityQueue<Task>::remove(GameClock::ClockTaskHndl hndl)
{
  auto task = std::find_if(m_container.begin(), m_container.end(), [hndl](auto it) { return it.hndl == hndl; });

  if (task != m_container.end())
  {
    this->m_container.erase(task);
    std::make_heap(m_container.begin(), m_container.end(), std::greater<Task>{});
    return true;
  }

  return false;
}

template <typename Task> void GameClock::RemovablePriorityQueue<Task>::clear(void) { m_container.clear(); }
