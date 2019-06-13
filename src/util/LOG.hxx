#ifndef LOG_HXX_
#define LOG_HXX_

#define STRINGIFY(x) #x
#define STRINGIFY2(x) STRINGIFY(x)
#define __line__ STRINGIFY2(__LINE__)
#define __RED__ "\33[38;5;196m"
#define __BLUE__ "\33[38;5;39m"
#define __YELLOW__ "\33[38;5;214m"
#define __PURPLE__ "\33[38;5;57m"
#define __CLEAR__ "\33[0m"
#define __BOLD__ "\33[1m"

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <SDL.h>

enum logType
{
  LOG_INFO,
  LOG_ERROR,
  LOG_EXCEPTION,
  LOG_DEBUG
};

class LOG
{

public:

  LOG(logType type)
  {
    _logType = type;
    logMessage << getTimeStamp();

    switch(_logType)
    {
      case LOG_ERROR:
        {
          logMessage << " - " __RED__ __BOLD__ "[ERROR]" __CLEAR__ " - ";
          break;
        }
      case LOG_INFO:
        {
          logMessage << " - " __BLUE__ __BOLD__ "[INFO]" __CLEAR__ " - ";
          break;
        }
      case LOG_EXCEPTION:
        {
          logMessage << " - " __YELLOW__ __BOLD__ "[EXCEPTION]" __CLEAR__ " - ";
          break;
        }
      case LOG_DEBUG:
        {
          #ifdef DEBUG
          logMessage << " - " __PURPLE__ __BOLD__ "[DEBUG]" __CLEAR__ " - ";
          break;
          #endif
        }
    }
  }

  ~LOG()
  {
    if (active)
    {
      std::cout << std::endl;
    }

    logMessage << " - ";
    active = false;
  }

  template <class T> LOG &operator<<(const T &msg)
  {
    logMessage << msg;
    std::cout << logMessage.str();

    if (_logType == LOG_ERROR)
    {
      writeErrorLog(logMessage.str());
    }

    // make sure the stringstream is clean before the operator is called again.
    logMessage.str(std::string());
    active = true;

    return *this;
  }

private:
  bool active = false;
  logType _logType = LOG_INFO;
  std::ostringstream logMessage;

  /** Gets formatted TimeStamp 
  * @return std::string containing current TimeStamp*/
  inline std::string getTimeStamp()
  {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S", std::localtime(&now));
    return buf;
  }

  /** Write log message to error.log file
  * @param message to write to error logfile
  */
  inline void writeErrorLog(const std::string &errorMessage)
  {
    std::string filePath = "error.log";
    std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    ofs << getTimeStamp() << '\t' << errorMessage << '\n';
    ofs.close();
  }
};

#endif
