#include "timer.hxx"

#include "../basics/log.hxx"

Timer::~Timer()
{
  _threadRunning = false;
  if (_timerThread.joinable())
  {
    _timerThread.join();
  }
}

void Timer::start()
{
  _startTime = SDL_GetPerformanceCounter();
  _lastTimeOutTime = SDL_GetPerformanceCounter();
  _endTime = _startTime;
  _elapsedTime = 0;
  _isActive = true;
  _timeOut = false;

  startThread();
}

void Timer::stop()
{
  _elapsedTime = (int)((SDL_GetPerformanceCounter() - _startTime) * 1000 / SDL_GetPerformanceFrequency());
  _isActive = false;
  _timeOut = false;
  _threadRunning = false;
  if (_timerThread.joinable())
  {
    _timerThread.join();
  }
}

void Timer::pause()
{
  if (_isActive)
  {
    _elapsedTime = (int)((SDL_GetPerformanceCounter() - _startTime) * 1000 / SDL_GetPerformanceFrequency());
    _timeSinceLastTimeOut = (int)((SDL_GetPerformanceCounter() - _lastTimeOutTime) * 1000 / SDL_GetPerformanceFrequency());
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
    _lastTimeOutTime = SDL_GetPerformanceCounter() - _lastTimeOutTime;
    _isActive = true;
  }
}

void Timer::setTimer(int timeInMs)
{
  _timeUntilTimeOut = timeInMs;
  _timeOut = false;
}

int Timer::getElapsedTime()
{
  if (_isActive)
  {
    _elapsedTime = (int)((SDL_GetPerformanceCounter() - _startTime) * 1000 / SDL_GetPerformanceFrequency());
  }
  return _elapsedTime;
}

int Timer::getElapsedTimeSinceLastTimeOut()
{
  if (_isActive)
  {
    _timeSinceLastTimeOut = (int)((SDL_GetPerformanceCounter() - _lastTimeOutTime) * 1000 / SDL_GetPerformanceFrequency());
  }
  return _timeSinceLastTimeOut;
}

void Timer::timeOut()
{
  _timeSinceLastTimeOut = 0;
  _timeOut = true;
  _timeOutSignal.emit();
  _lastTimeOutTime = SDL_GetPerformanceCounter();
}

void Timer::startThread()
{
  // if thread is running, abort and join it before starting a new one
  if (_timerThread.joinable())
  {
    _threadRunning = false;
    _timerThread.join();
  }
  else if (_threadRunning == true)
  {
    return;
  }

  _threadRunning = true;
  if (_timeUntilTimeOut != 0)
  {
    _timerThread = std::thread([=]() {
      while (_threadRunning)
      {
        if (getElapsedTimeSinceLastTimeOut() >= _timeUntilTimeOut)
        {
          timeOut();
          if (!_loopTimer)
          {
            _threadRunning = false;
          }
        }

        // helps with cpu usage...
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      return;
    });
  }
}