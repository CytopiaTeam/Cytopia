#ifndef TIMER_HXX_
#define TIMER_HXX_

#include <ctime>
#include <chrono>
#include <thread>
#include <future>

#include "SDL.h"
#include "../basics/log.hxx"
#include "../basics/signal.hxx"

class Timer
{
public:
  /** \brief Timer object
  * creates a Timer object, that can measure past time in milliseconds or set a 
  * countdown timer. It's also possible to invoke a callback function when the timer is triggered
  */
  Timer() = default;
  ~Timer();

  /** \brief get elapsed time
  * Returns the time in milliseconds, that has passed since the timer was started.
  * @returns the time in milliseconds, that has passed since the timer was started
  */
  int getElapsedTime();

  /** \brief get elapsed time since last timeOut
  * Returns the time in milliseconds, that has passed since the last TimeOut has happend.
  * @returns the time in milliseconds, that has passed since the last timeOut
  * @note Only applicable, if a timer has been set
  * @see Timer#setTimer()
  */
  int getElapsedTimeSinceLastTimeOut();

  /** \brief Start / Reset the timer
  * Starts or restarts the timer.
  * If the timer is already running, it will be stopped and restarted.
  */
  void start();

  /** Stop the timer
  * Stops the timer and set it back to 0 
  */
  void stop();

  /** Pauses the timer
   */
  void pause();

  /** Resume the timer
  * Resumed the paused timer. If the timer is not paused, it will behave as start()
  */
  void resume();

  /** \brief Checks if the timer is active.
  * Returns true if the timer is running (pending); otherwise returns false.
  */
  bool isActive() { return _isActive; };

  /** \brief Checks if the time is done.
  * Returns true, if the timer has reached 0. 
  * @note Only applicable, if a timer has been set
  * @see Timer#setTimer()
  */
  bool isTimedOut() { return _timeOut; };

  /** \brief Sets a timer
   * Sets a timer. When a timer is set, a signal is emitted and the registered callback function 
   * is called. Also a bool variable will be set to true.
   * @see Timer#egisterCallbackFunction
   * @see Timer#isTimedOut()
  */
  void setTimer(int timeInMs);

  /** \brief Register callback function for this timer
  * Register one or more callback functions that should be exececuted, when the Timer is triggered.
  * @note Only applicable, if a timer has been set
  * @see setTimer()
  * @see Signal#Signal
  * @see Signal#slot
  */
  void registerCallbackFunction(std::function<void()> const &cb) { _timeOutSignal.connect(cb); };

  /** Loop Timer
  * Specify if the timerout callback is called once or everytime the counter hits zero.
  */
  void loopTimer(bool loop) { _loopTimer = loop; };

private:
  void startThread();

  Signal::Signal<void()> _timeOutSignal;

  std::thread _timerThread;

  bool _loopTimer = false;
  bool _timeOut = false;
  bool _isActive = false;
  bool _threadRunning = false;

  int _timeUntilTimeOut = 0;
  int _timeSinceLastTimeOut = 0;
  int _elapsedTime = 0;

  Uint64 _startTime = 0;
  Uint64 _endTime = 0;

  Uint64 _lastTimeOutTime = 0;

  void timeOut();
};

#endif