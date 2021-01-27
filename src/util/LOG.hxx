#ifndef LOG_HXX_
#define LOG_HXX_

#include <string>
#include "Exception.hxx"

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

/**
 * @def debug_scope
 * Creates a scope which only gets
 * executed for debug builds
 */
#ifdef DEBUG
#define debug_scope
#else
#define debug_scope if constexpr(false)
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

  static constexpr const char *LOG_PREFIX_COLORED[] = {
      " - \x1B[38;5;39m\x1B[1m[INFO]\x1B[0m - ",     " - \x1B[38;5;57m\x1B[1m[DEBUG]\x1B[0m - ",
      " - \x1B[38;5;214m\x1B[1m[WARNING]\x1B[0m - ", " - \x1B[38;5;196m\x1B[1m[ERROR]\x1B[0m - ",
      " - \x1B[38;5;124m\x1B[1m[FATAL]\x1B[0m - ",
  };

  static constexpr const char *LOG_PREFIX[] = {
      " - [INFO] - ", " - [DEBUG] - ", " - [WARNING] - ", " - [ERROR] - ", " - [FATAL] - ",
  };

public:
  /* 1 MB */
  using MAX_LOG_SIZE_BYTES = Constant<1000000>;

  /**
   * @brief Create a Logger
   */
  LOG(LogType type);

  /**
   * @brief Destroy a Logger
   */
  ~LOG();

  /**
   * @brief Log a message
   * @tparam Object the streamable message type
   */
  template <class Object> LOG &operator<<(const Object &msg);

private:
  LogType m_LogType;
  std::ostringstream m_Logger;

  /** @brief Gets formatted TimeStamp
   *  Gets formatted TimeStamp
   *  @return std::string containing current TimeStamp
   */
  std::string getTimeStamp();

  /** @brief Write log message to error.log file
   *  @details If the log file exceeds MAX_LOG_SIZE_BYTES, logs will be cut in half
   *  @param message to write to error logfile
   */
  void writeErrorLog(const std::string &errorMessage);
};

#include "LOG.inl.hxx"

#undef _CRT_SECURE_NO_WARNINGS

#endif
