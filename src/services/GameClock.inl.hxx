#include "LOG.hxx"

template <typename DelayType, typename PeriodType>
inline GameClock::ClockTaskHndl GameClock::addRealTimeClockTask(ClockCbk cbk, DelayType delay, PeriodType period)
{
  if ((cbk != nullptr) && (TimePoint(delay) > TimePointZero) && (TimePoint(period) >= TimePointZero))
  {
    std::lock_guard<std::mutex> lock(m_lock);

    m_realTimeTasks.add(RealTimeClockTask(cbk, Clock::now() + (TimePoint(delay) - TimePointZero),
                                          (TimePoint)period - TimePointZero, ++m_unique_handle));
    return m_unique_handle;
  }
  else
  {
    return ClockTaskHndlInvalid;
  }
}

template <typename Task, typename Now> void GameClock::tickTask(PriorityQueue<Task> &queue, Now now)
{
  while (!queue.isEmpty() && (now >= queue.top().m_waketime))
  {
    const auto task = queue.top();
    queue.pop();
    const bool isCanceled = task.callback();

    if (!isCanceled && (task.m_period != decltype(task.m_period){0}))
    {
      queue.add(Task(task.callback, now + task.m_period, task.m_period, task.hndl));
    }
  }
}
