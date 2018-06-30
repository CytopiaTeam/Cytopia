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

  double getDeltaTime();
  void start();
  void reset();

private:
  Uint64 now = 0;
  Uint64 last = 0;
  double deltaTime = 0;
};

#endif