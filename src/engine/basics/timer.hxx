#ifndef TIMER_HXX_
#define TIMER_HXX_

#include <ctime>
#include <chrono>

#include "SDL.h"

class Timer
{
public:
  Timer() = default;
  ~Timer() = default;

  double getDeltaTime() { return deltaTime; };
  void start();
  void stop();

private:
  Uint64 now;
  Uint64 last;
  double deltaTime;
};

#endif