#ifndef GAME_CLOCK_HXX_
#define GAME_CLOCK_HXX_

#include <chrono>
#include <mutex>
#include <functional>
#include "../util/PriorityQueue.hxx"
#include "../util/Singleton.hxx"

using namespace std::chrono_literals;

/**
 * @brief Game clock service. Implement two timers one real time timer and other game time timer.
 * @details Both timers provide possibility to add task which will be triggered after delay time run out.
 * Game timer represent timer running in game time.
 * The game time can be scaled providing possibility to speed up or slow down game.
 */
class GameClock : public Singleton<GameClock>
{
private:
  using Clock = std::chrono::high_resolution_clock;
  template <typename T, typename Comparator> friend class PriorityQueue;

public:
  using TimePoint = std::chrono::time_point<Clock>;
  using TimeDuration = Clock::duration;
  using ClockCbk = std::function<bool(void)>;
  using ClockTaskHndl = unsigned long;
  using GameClockTime = unsigned long;
  using GameClockDuration = unsigned long;

  /**
  * @brief Represent 1 minute of game time.
  */
  static constexpr GameClockTime GameMinute = 1;

  /**
  * @brief Represent 1 hour of game time.
  */
  static constexpr GameClockTime GameHour = 60 * GameMinute;

  /**
  * @brief Represent 1 day of game time.
  */
  static constexpr GameClockTime GameDay = 24 * GameHour;

  /**
  * @brief This function provides the tick for both clocks.
  * @details It must be called frequently. Call frequency determines clock precision.
  */
  void tick(void);

  /**
  * @brief Add new real time clock task.
  * @param cbk Callback function to be called after delay time is passed.
  * @param delay Delay in chrono literals (e.g. 1h, 2min, 3s ...), callback function will be called after delay timer is passed.
  * @param period Repeat period in chrono literals (e.g. 1h, 2min, 3s ...) the timer will be reset again with new delay time in amount of period time.
  * @return Real time clock task handle. Can be used to remove clock and verify whether it is started correctly.
  *         In case of failure #ClockTaskHndlInvalid will be return.
  */
  template <typename DelayType, typename PeriodType = TimePoint>
  GameClock::ClockTaskHndl addRealTimeClockTask(ClockCbk cbk, DelayType delay, PeriodType period = TimePointZero);

  /**
  * @brief Add new game time clock task.
  * @param cbk Callback function to be called after delay time is passed.
  * @param delay Delay in game timer ticks. Use provided values GameDay, GameHour, GameMinute and scale it as necessary.
  *        Callback function will be called after delay timer is passed.
  * @param period Repeat period in game timer ticks. Use provided values GameDay, GameHour, GameMinute and scale it as necessary.
  *        The timer will be reset again with new delay ticks in amount of period ticks.
  * @return Game time clock task handle. Can be used to remove clock and verify whether it is started correctly.
  *         In case of failure #ClockTaskHndlInvalid will be return.
  */
  GameClock::ClockTaskHndl addGameTimeClockTask(ClockCbk cbk, GameClockTime delay, GameClockTime period = 0U);

  /**
  * @brief Set game clock speed.
  * @param speedFactor Game clock scale factor.
  * E.g. to run game clock 4 time faster provide 4.0f.
  */
  void setGameClockSpeed(float speedFactor);

  float getGameClockSpeed() const { return m_speedFactor; }

  /**
  * @brief Remove all real time and game time clocks.
  */
  void clear(void);

  /**
  * @brief Remove real/game time clock.
  *        After it is removed successfully it is guaranteed it will not trigger callback
  * @param hndl Handle of clock which should be removed.
  * @return true in case clock is successfully removed, otherwise false.
  */
  bool removeClockTask(ClockTaskHndl hndl);

private:
  /**
  * @brief Duration of default game timer tick in ms.
  */
  static constexpr unsigned int DefaultGameTickDuration = 2000;
  static constexpr TimePoint TimePointZero = TimePoint{0s};
  /**
  * @brief Invalid task handler. In case that clock is not added.
  */
  static constexpr ClockTaskHndl ClockTaskHndlInvalid = ClockTaskHndl{0};

  /**
  * @brief Template structure provide base for different clock tasks.
  */
  template <typename Time, typename Duration> struct ClockTask
  {
    ClockCbk callback;
    Time m_waketime;
    Duration m_period;
    ClockTaskHndl hndl;

    explicit ClockTask(ClockCbk cbk, Time delay, Duration period, ClockTaskHndl hndl)
        : callback{cbk}, m_waketime{delay}, m_period{period}, hndl{hndl}
    {
    }

    bool operator==(const ClockTask &task2) const { return hndl == task2.hndl; };
    bool operator>(const ClockTask &task2) const { return m_waketime > task2.m_waketime; }
  };

  using RealTimeClockTask = ClockTask<TimePoint, TimeDuration>;
  using GameTimeClockTask = ClockTask<GameClockTime, GameClockDuration>;

  PriorityQueue<RealTimeClockTask, std::greater<RealTimeClockTask>> m_realTimeTasks;
  PriorityQueue<GameTimeClockTask, std::greater<GameTimeClockTask>> m_gameTimeTasks;
  std::mutex m_lock;
  // Provide way to return unique handle for each task.
  ClockTaskHndl m_unique_handle = 0U;
  /// Current number of the game ticks.
  GameClockTime m_gameTicks = 0U;
  /// Last time of the game tick.
  TimePoint m_lastGameTickTime = Clock::now();
  /// The current game tick duration on milliseconds.

  float m_speedFactor = 1.f;
  Clock::duration m_gameTickDuration = std::chrono::milliseconds(DefaultGameTickDuration);

  /**
  * @brief Tick clock for given task type.
  * @param queue Priority queue of tasks.
  * @param now The time point when tick occurred.
  */
  template <typename Task, typename Cmp, typename Now> void tickTask(PriorityQueue<Task, Cmp> &queue, Now now);
};

#include "GameClock.inl.hxx"

#endif // GAME_CLOCK_HXX_
