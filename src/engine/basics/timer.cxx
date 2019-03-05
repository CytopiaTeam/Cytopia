#include "timer.hxx"

#include "../basics/log.hxx"

Timer::~Timer()
{
  m_threadRunning = false;
  if (m_timerThread.joinable())
  {
    m_timerThread.join();
  }
}

void Timer::start()
{
  m_startTime = SDL_GetPerformanceCounter();
  m_lastTimeOutTime = SDL_GetPerformanceCounter();
  m_endTime = m_startTime;
  m_elapsedTime = 0;
  m_isActive = true;
  m_timeOut = false;

  startThread();
}

void Timer::stop()
{
  m_elapsedTime = static_cast<int>((SDL_GetPerformanceCounter() - m_startTime) * 1000 / SDL_GetPerformanceFrequency());
  m_isActive = false;
  m_timeOut = false;
  m_threadRunning = false;
  if (m_timerThread.joinable())
  {
    m_timerThread.join();
  }
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

void Timer::startThread()
{
  // if thread is running, abort and join it before starting a new one
  if (m_timerThread.joinable())
  {
    m_threadRunning = false;
    m_timerThread.join();
  }
  else if (m_threadRunning == true)
  {
    return;
  }

  m_threadRunning = true;
  if (m_timeUntilTimeOut != 0)
  {
    m_timerThread = std::thread([=]() {
      while (m_threadRunning)
      {
        if (getElapsedTimeSinceLastTimeOut() >= m_timeUntilTimeOut)
        {
          timeOut();
          if (!m_loopTimer)
          {
            m_threadRunning = false;
          }
        }

        // helps with cpu usage...
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      return;
    });
  }
}