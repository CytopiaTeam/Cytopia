#include "Timer.hxx"

#include "LOG.hxx"
#include "../EventManager.hxx"

Timer::~Timer()
{
  stop();
  //EventManager::instance().removeTimer(this);
}

void Timer::start()
{
  m_startTime = SDL_GetPerformanceCounter();
  m_lastTimeOutTime = SDL_GetPerformanceCounter();
  m_endTime = m_startTime;
  m_elapsedTime = 0;
  m_isActive = true;
  m_timeOut = false;

  EventManager::instance().registerTimer(this);
}

void Timer::stop()
{
  m_elapsedTime = static_cast<int>((SDL_GetPerformanceCounter() - m_startTime) * 1000 / SDL_GetPerformanceFrequency());
  m_isActive = false;
  m_timeOut = false;
}

void Timer::pause()
{
  if (m_isActive)
  {
    m_elapsedTime = static_cast<int>((SDL_GetPerformanceCounter() - m_startTime) * 1000 / SDL_GetPerformanceFrequency());
    m_timeSinceLastTimeOut =
        static_cast<int>((SDL_GetPerformanceCounter() - m_lastTimeOutTime) * 1000 / SDL_GetPerformanceFrequency());
    m_endTime = m_startTime;
    m_startTime = SDL_GetPerformanceCounter();
    m_isActive = false;
  }
}

void Timer::resume()
{
  if (!m_isActive)
  {
    m_startTime = SDL_GetPerformanceCounter() - (m_startTime - m_endTime);
    m_lastTimeOutTime = SDL_GetPerformanceCounter() - m_lastTimeOutTime;
    m_isActive = true;
  }
}

void Timer::setTimer(int timeInMs)
{
  m_timeUntilTimeOut = timeInMs;
  m_timeOut = false;
}

int Timer::getElapsedTime()
{
  if (m_isActive)
  {
    m_elapsedTime = static_cast<int>((SDL_GetPerformanceCounter() - m_startTime) * 1000 / SDL_GetPerformanceFrequency());
  }
  return m_elapsedTime;
}

int Timer::getElapsedTimeSinceLastTimeOut()
{
  if (m_isActive)
  {
    m_timeSinceLastTimeOut =
        static_cast<int>((SDL_GetPerformanceCounter() - m_lastTimeOutTime) * 1000 / SDL_GetPerformanceFrequency());
  }
  return m_timeSinceLastTimeOut;
}

void Timer::timeOut()
{
  m_timeSinceLastTimeOut = 0;
  m_timeOut = true;
  m_timeOutSignal.emit();
  m_lastTimeOutTime = SDL_GetPerformanceCounter();
}

void Timer::checkTimeout()
{
  if (getElapsedTimeSinceLastTimeOut() >= m_timeUntilTimeOut)
  {
    timeOut();
    if (!m_loopTimer)
    {
      stop();
    }
  }
}
