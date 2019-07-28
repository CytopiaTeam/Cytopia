#ifndef TIMER_HXX_
#define TIMER_HXX_

#include <ctime>
#include <chrono>
#include <thread>

#include <SDL.h>
#include "../basics/signal.hxx"

class Timer
{
public:
  /** \brief Timer object
  * creates a Timer object, that can measure past time in milliseconds or set a 
  * countdown timer. It's also possible to invoke a callback function when the timer is triggered
  */
  Timer() = default;
  
  /**
  * @brief Destroys the Timer object
  */
  ~Timer();

  /** \brief get elapsed time
  * Returns the time in milliseconds, that has passed since the timer was started.
  * @returns the time in milliseconds, that has passed since the timer was started
  */
  int getElapsedTime();

  /** \brief get elapsed time since last timeOut
  * Returns the time in milliseconds, that has passed since the last TimeOut has happened.
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

  /** \brief Stop the timer
  * Stops the timer and set it back to 0 
  */
  void stop();

  /** \brief Pauses the timer
   * Pauses the timer.
   */
  void pause();

  /** \brief Resume the timer
  * Resumed the paused timer. If the timer is not paused, it will behave as start()
  */
  void resume();

  /** \brief Checks if the timer is active.
  * Returns true if the timer is running (pending); otherwise returns false.
  * @returns true if the timer is running, false if it is not
  */
  bool isActive() { return m_isActive; };

  /** \brief Checks if the time is done.
  * Returns true, if the timer has reached 0. 
  * @returns if the timer has reached 0 or not
  * @note Only applicable, if a timer has been set
  * @see Timer#setTimer()
  */
  bool isTimedOut() { return m_timeOut; };

  /** \brief Sets a timer
   * Sets a timer. When a timer is set, a signal is emitted and the registered callback function 
   * is called. Also a bool variable will be set to true.
   * @param the time in milliseconds the timer will be set to
   * @see Timer#registerCallbackFunction
   * @see Timer#isTimedOut()
  */
  void setTimer(int timeInMs);

  /** \brief Register callback function for this timer
  * Register one or more callback functions that should be executed, when the Timer is triggered.
  * @param the callback functions to register
  * @note Only applicable if a timer has been set
  * @see setTimer()
  * @see Signal#Signal
  * @see Signal#slot
  */
  void registerCallbackFunction(std::function<void()> const &cb) { m_timeOutSignal.connect(cb); };

  /** \brief Loop Timer
  * Specify if the timeout callback is called once or everytime the counter hits zero.
  * @param true if the callback is supposed to loop, false if it is supposed to be called once
  */
  void loopTimer(bool loop) { m_loopTimer = loop; };

  void checkTimeout();

private:
  Signal::Signal<void()> m_timeOutSignal;

  bool m_loopTimer = false;
  bool m_timeOut = false;
  bool m_isActive = false;

  /// time until the next TimeOut.
  int m_timeUntilTimeOut = 0;
  /// time that has passed since the last TimeOut.
  int m_timeSinceLastTimeOut = 0;
  /// time that has passed since the timer was started.
  int m_elapsedTime = 0;

  /// the time at whichthe timer was started.
  Uint64 m_startTime = 0;
  /// the time at which the timer was stopped.
  Uint64 m_endTime = 0;
  /// the time of the last TimeOut.
  Uint64 m_lastTimeOutTime = 0;

  void timeOut();
};

#endif
