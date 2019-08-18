#ifndef LOG_HXX_
#define LOG_HXX_

#include <string>

#define STRINGIFY(x) #x
#define STRINGIFY2(x) STRINGIFY(x)
#define __line__ STRINGIFY2(__LINE__)
#define TRACE_INFO "Exception thrown from " + string(__PRETTY_FUNCTION__) + " at " __FILE__ ":" __line__ " - "
// Required to use std::getenv without warning on MSVC
#define _CRT_SECURE_NO_WARNINGS

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif __INTEL_COMPILER
#define __PRETTY_FUNCTION__ "Unknown_Method"
#endif

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <algorithm>
#include <mutex>

#include "Meta.hxx"
#include "filesystem.hxx"

using std::string;
using std::stringstream;
using Mutex = std::mutex;
using LockGuard = std::lock_guard<Mutex>;

enum LogType
{
  LOG_INFO = 0,
  LOG_DEBUG,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL,
};

class LOG
{

  /* Mutex */
  static Mutex StreamMutex;
  
  static constexpr const char * LOG_PREFIX_COLORED[] = {
    " - \x1B[38;5;39m\x1B[1m[INFO]\x1B[0m - ",
    " - \x1B[38;5;57m\x1B[1m[DEBUG]\x1B[0m - ",
    " - \x1B[38;5;214m\x1B[1m[WARNING]\x1B[0m - ",
    " - \x1B[38;5;196m\x1B[1m[ERROR]\x1B[0m - ",
    " - \x1B[38;5;124m\x1B[1m[FATAL]\x1B[0m - ",
  };
  
  static constexpr const char * LOG_PREFIX[] = {
    " - [INFO] - ",
    " - [DEBUG] - ",
    " - [WARNING] - ",
    " - [ERROR] - ",
    " - [FATAL] - ",
  };

public:

  /* 1 MB */
  using MAX_LOG_SIZE_BYTES = Constant<1000000>;

  LOG(LogType type) : m_LogType(type) { }

  ~LOG()
  { 
    LockGuard lock(StreamMutex);
    string Message = getTimeStamp() + LOG_PREFIX[m_LogType] + m_Logger.str();
    if(!getenv("TERM"))
    {
      #ifndef DEBUG
      if(m_LogType != LOG_DEBUG)
      #endif
        std::cout << Message << std::endl;
    }
    else
    {
      #ifndef DEBUG
      if(m_LogType != LOG_DEBUG)
      #endif
        std::cout << getTimeStamp() << LOG_PREFIX_COLORED[m_LogType] << m_Logger.str() << std::endl;
    }
    writeErrorLog(Message);
  }

  template <class Object> LOG &operator<<(const Object &msg)
  {
    m_Logger << msg;
    return *this;
  }

private:
  LogType m_LogType;
  std::ostringstream m_Logger;

  /** @brief Gets formatted TimeStamp
   *  Gets formatted TimeStamp
   *  @return std::string containing current TimeStamp
   */
  inline std::string getTimeStamp()
  {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S", std::gmtime(&now)); // lgtm [cpp/potentially-dangerous-function]
    return buf;
  }

  /** @brief Write log message to error.log file
   *  @details If the log file exceeds MAX_LOG_SIZE_BYTES, logs will be cut in half
   *  @param message to write to error logfile
   */
  inline void writeErrorLog(const std::string &errorMessage)
  {
    std::fstream fs(SDL_GetBasePath() + string{"error.log"}, std::fstream::ate | std::fstream::in | std::fstream::out);
    fs.exceptions(std::fstream::failbit | std::fstream::badbit);
    fs << errorMessage << std::endl;
    fs.seekg(0, fs.end);
    std::streampos Size = fs.tellg();
    if(Size > MAX_LOG_SIZE_BYTES::value)
    {
      fs.seekp(0);
      string line;
      std::streampos Cut = 0;
      while(Size - Cut > MAX_LOG_SIZE_BYTES::value / 2 && std::getline(fs, line))
        Cut += line.size() + 1;
      stringstream truncatedstream;
      truncatedstream << fs.rdbuf();
      fs.close();
      fs.open(SDL_GetBasePath() + string{"error.log"}, std::fstream::trunc | std::fstream::out);
      fs << truncatedstream.str();
    }
  }

};

#undef _CRT_SECURE_NO_WARNINGS

#endif
