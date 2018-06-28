#include "timer.hxx"

void Timer::start()
{
  now = SDL_GetPerformanceCounter();
  last = now;
}

void Timer::stop()
{
  last = now;
  now = SDL_GetPerformanceCounter();

  deltaTime = (double)((now - last) * 1000 / SDL_GetPerformanceFrequency());
}