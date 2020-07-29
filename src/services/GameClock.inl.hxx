#include "GameClock.hxx"

template <typename Duration> void GameClock::createDefferedTask(Duration &&duration, Callback callback)
{
  LockGuard lock(m_lock);
  auto future = duration + std::chrono::system_clock::now();
  m_Deffered.emplace(DefferedTask{callback, future.time_since_epoch().count()});
}

template <typename Duration> inline void GameClock::createRepeatedTask(Duration &&duration, Callback callback)
{
  LockGuard lock(m_lock);
  auto future = std::chrono::system_clock::now();
  m_Repeated.emplace(RepeatedTask{callback, future.time_since_epoch().count(), ClockDuration{duration}.count()});
}
