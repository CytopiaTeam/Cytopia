#include "timer.hxx"

void Timer::start()
{
  now = SDL_GetPerformanceCounter();
  last = now;
}

void Timer::reset()
{
  last = now;
  now = SDL_GetPerformanceCounter();

  deltaTime = (double)((now - last) * 1000 / SDL_GetPerformanceFrequency());
}

double Timer::getDeltaTime()
{

  deltaTime = (double)((SDL_GetPerformanceCounter() - now) * 1000 / SDL_GetPerformanceFrequency());
  return deltaTime;
};
