#ifndef GAME_CLOCK_HXX_
#define GAME_CLOCK_HXX_

#include <queue>
#include <thread>
#include <chrono>

#include "../GameService.hxx"

using Thread = std::thread;
using Mutex = std::mutex;
using LockGuard = std::lock_guard<Mutex>;
using namespace std::chrono_literals;

using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;
using GameDay = std::chrono::duration<int, std::ratio<2592>>;
using HalfDay = std::chrono::duration<int, std::ratio<1296>>;

class GameClock : public GameService
{

  using TimeRep = std::chrono::system_clock::rep;

  struct DefferedTask
  {
    std::function<void(void)> callback;
    TimeRep waketime;
    inline friend bool operator>(const DefferedTask &dt1, const DefferedTask &dt2) { return dt1.waketime > dt2.waketime; }
  };

  std::priority_queue<DefferedTask, std::vector<DefferedTask>, std::greater<DefferedTask>> m_queue;
  Mutex m_lock;
  Thread m_thread{&GameClock::clockLoop, this};

  void clockLoop();

public:

  GameClock(GameService::ServiceTuple &);
  ~GameClock();

  using Callback = std::function<void(void)>;

  /**
 	 *  @brief Registers a new Timer
   *  @details After duration time, callback will be called
   *  @tparam Duration the duration type
   *  @param duration the Duration
   *  @param callback the function to call at schedule
   */
  template <typename Duration> void registerTimer(Duration &&, Callback);
};

#include "GameClock.inl.hxx"

#endif
