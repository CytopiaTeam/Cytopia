#include "GameClock.hxx"
#include "../util/LOG.hxx"

GameClock::GameClock(GameService::ServiceTuple & services) : 
  GameService(services)
{ }

GameClock::~GameClock()
{
  registerTimer(0s, Callback{nullptr});
  m_thread.join();
}

void GameClock::clockLoop()
{
  for (;;)
  {
    {
      LockGuard lock(m_lock);
      auto now = std::chrono::system_clock::now().time_since_epoch().count();
      while (!m_queue.empty() and m_queue.top().waketime < now)
      {
        if (m_queue.top().callback == nullptr)
          return;
        m_queue.top().callback();
        m_queue.pop();
      }
    }
    std::this_thread::sleep_for(1s);
  }
}
