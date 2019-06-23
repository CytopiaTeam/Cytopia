#ifndef LOG_HXX_
#define LOG_HXX_

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>

#include <SDL.h>

enum logType
{
  LOG_INFO,
  LOG_ERROR
};

class LOG
{
public:
  LOG()
  {
    logMessage << getTimeStamp();
    logMessage << " - ";
  }

  LOG(logType type)
  {
    _logType = type;
    logMessage << getTimeStamp();

    if (_logType == LOG_ERROR)
    {
      logMessage << " - ERROR - ";
    }
    else
    {
      logMessage << " - ";
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

  /** @brief Gets formatted TimeStamp
  * Gets formatted TimeStamp
  * @return std::string containing current TimeStamp*/
  inline std::string getTimeStamp()
  {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S", std::localtime(&now));
    return buf;
  }

  /** @brief Write log message to error.log file
  * Write log message to error.log file
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
