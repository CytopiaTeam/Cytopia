#include "LOG.hxx"
#include "Constants.hxx"
#include "Filesystem.hxx"
#include "Settings.hxx"

#ifdef __ANDROID__
#include <android/log.h>
#endif

Mutex LOG::StreamMutex;

LOG::LOG(LogType type) : m_LogType(type) {}

LOG::~LOG()
{
  LockGuard lock(StreamMutex);
  string message = getTimeStamp() + LOG_PREFIX[m_LogType] + m_Logger.str();
  if (!getenv("TERM"))
  {
#ifndef DEBUG
    if (m_LogType != LOG_DEBUG)
#endif
#ifdef __ANDROID__
      __android_log_print(ANDROID_LOG_INFO, "Cytopia", "%s", message.c_str());
#else
    std::cout << message << std::endl;
#endif
  }
  else
  {
#ifndef DEBUG
    if (m_LogType != LOG_DEBUG)
#endif
      std::cout << getTimeStamp() << LOG_PREFIX_COLORED[m_LogType] << m_Logger.str() << std::endl;
  }
#ifndef __ANDROID__
  writeErrorLog(message);
#endif
}

const std::string LOG::getTimeStamp()
{
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char buf[100] = {0};
  std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S", std::localtime(&now)); // lgtm [cpp/potentially-dangerous-function]
  return buf;
}

void LOG::writeErrorLog(const std::string &errorMessage) const
{
  fs::createDirectory(CYTOPIA_DATA_DIR);
  string errfname = CYTOPIA_DATA_DIR + string{"error.log"};

  /* First we create the file if it doesn't exist */
  std::fstream fs(errfname, std::fstream::out | std::fstream::app);
  if (!fs)
    throw CytopiaError(TRACE_INFO "Could not open file " + errfname);

  /* We send the message */
  fs << errorMessage << std::endl;

  /* We compute the size of the file */
  fs.seekp(0, std::fstream::end);
  std::streampos Size = fs.tellp();
  if (Size > MAX_LOG_SIZE_BYTES::value)
  {
    /* We need to rotate the logs */
    std::fstream fsToRotate(errfname, std::fstream::in | std::fstream::out);
    fsToRotate.seekg(0);
    string line;
    std::streampos Cut = 0;
    while (Size - Cut > MAX_LOG_SIZE_BYTES::value / 2 && std::getline(fsToRotate, line))
      Cut += line.size() + 1;
    stringstream truncatedstream;
    truncatedstream << fsToRotate.rdbuf();
    fsToRotate.close();
    fsToRotate.open(fs::getBasePath() + string{"error.log"}, std::fstream::trunc | std::fstream::out);
    fsToRotate << truncatedstream.str();
  }
}
