#include "timer.hxx"

void Timer::start()
{
  _startTime = SDL_GetPerformanceCounter();
  _endTime = _startTime;
  _elapsedTime = 0;
  _isActive = true;
}

void Timer::stop()
{
  _elapsedTime = (double)((SDL_GetPerformanceCounter() - _startTime) * 1000 / SDL_GetPerformanceFrequency());
  _isActive = false;
}

void Timer::pause()
{
  if (_isActive)
  {
    _elapsedTime = (double)((SDL_GetPerformanceCounter() - _startTime) * 1000 / SDL_GetPerformanceFrequency());
    _endTime = _startTime;
    _startTime = SDL_GetPerformanceCounter();
    _isActive = false;
  }
}

void Timer::resume()
{
  if (!_isActive)
  {
    _startTime = SDL_GetPerformanceCounter() - (_startTime - _endTime);
    _isActive = true;
  }
}

double Timer::getElapsedTime()
{
  if (_isActive)
  {
    _elapsedTime = (double)((SDL_GetPerformanceCounter() - _startTime) * 1000 / SDL_GetPerformanceFrequency());
  }
  return _elapsedTime;
};
