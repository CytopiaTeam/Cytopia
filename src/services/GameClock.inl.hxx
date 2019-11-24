
template <typename Duration> void GameClock::registerTimer(Duration &&duration, Callback callback)
{
  LockGuard lock(m_lock);
  auto future = duration + std::chrono::system_clock::now();
  m_queue.emplace(DefferedTask{callback, future.time_since_epoch().count()});
}
