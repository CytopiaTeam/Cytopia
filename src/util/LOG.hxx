#ifndef LOG_HXX_
#define LOG_HXX_

#define __line__ " ## __LINE__ ## "
#define RED "\e[38;5;196m"
#define BLUE "\e[38;5;39m"
#define YELLOW "\e[38;5;214m"
#define PURPLE "\e[38;5;57m"
#define CLEAR "\e[0m"
#define BOLD "\e[1m"

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
          logMessage << " - " RED BOLD "[ERROR]" CLEAR " - ";
          break;
        }
      case LOG_INFO:
        {
          logMessage << " - " BLUE BOLD "[INFO]" CLEAR " - ";
          break;
        }
      case LOG_EXCEPTION:
        {
          logMessage << " - " YELLOW BOLD "[EXCEPTION]" CLEAR " - ";
          break;
        }
      case LOG_DEBUG:
        {
          #ifdef DEBUG
          logMessage << " - " PURPLE BOLD "[DEBUG]" CLEAR " - ";
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
