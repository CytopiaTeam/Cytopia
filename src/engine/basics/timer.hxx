#ifndef TIMER_HXX_
#define TIMER_HXX_

#include <ctime>
#include <chrono>

#include "SDL.h"
#include "../basics/log.hxx"

class Timer
{
public:
  /** \brief Timer object
  * creates a Timer object, that can measure past time in milliseconds
  */
  Timer() = default;
  ~Timer() = default;

  /** \brief get elapsed time
  * Returns the time in milliseconds, that has passed since the timer was started.
  * @returns the time in milliseconds, that has passed since the timer was started
  */
  double getElapsedTime();

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

  /** \bref Checks if the timer is active.
  * Returns true if the timer is running (pending); otherwise returns false.
  */
  bool isActive() { return _isActive; };

private:
  Uint64 _startTime = 0;
  Uint64 _endTime = 0;
  double _elapsedTime = 0;
  bool _isActive = false;
};

#endif