#ifndef GAME_CLOCK_HXX_
#define GAME_CLOCK_HXX_

#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>

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

  using ClockRep = std::chrono::system_clock::rep;
  using ClockDuration = std::chrono::system_clock::duration;

  struct DefferedTask
  {
    std::function<void(void)> callback;
    ClockRep waketime;
    inline friend bool operator>(const DefferedTask &dt1, const DefferedTask &dt2) { return dt1.waketime > dt2.waketime; }
  };

  struct RepeatedTask
  {
    std::function<void(void)> callback;
    ClockRep waketime;
    ClockRep interval;
    inline friend bool operator>(const RepeatedTask &dt1, const RepeatedTask &dt2) { return dt1.waketime > dt2.waketime; }
  };

  using DefferedSchedule = std::priority_queue<DefferedTask, std::vector<DefferedTask>, std::greater<DefferedTask>>;
  using RepeatedSchedule = std::priority_queue<RepeatedTask, std::vector<RepeatedTask>, std::greater<RepeatedTask>>;

  DefferedSchedule m_Deffered;
  RepeatedSchedule m_Repeated;
  Mutex m_lock;
  Thread m_thread;

  void clockLoop();

public:
  GameClock(GameService::ServiceTuple &);
  ~GameClock();

  using Callback = std::function<void(void)>;

  /**
    *  @brief Registers a new deffered task
    *  @details After duration time, callback will be called
    *  @tparam Duration the duration type
    *  @param duration the Duration
    *  @param callback the function to call at schedule
	*/
  template <typename Duration> void createDefferedTask(Duration &&, Callback);

  /**
    *  @brief Registers a new repeated task
    *  @details Every duration time, callback will be called
    *  @tparam Duration the duration type
    *  @param duration the Duration
    *  @param callback the function to call at schedule
	*/
  template <typename Duration> void createRepeatedTask(Duration &&, Callback);
};

#include "GameClock.inl.hxx"

#endif
