#include "LOG.hxx"

Mutex LOG::StreamMutex;

LOG::LOG(LogType type) : m_LogType(type) {}

LOG::~LOG()
{
  LockGuard lock(StreamMutex);
  string Message = getTimeStamp() + LOG_PREFIX[m_LogType] + m_Logger.str();
  if (!getenv("TERM"))
  {
#ifndef DEBUG
    if (m_LogType != LOG_DEBUG)
#endif
      std::cout << Message << std::endl;
  }
  else
  {
#ifndef DEBUG
    if (m_LogType != LOG_DEBUG)
#endif
      std::cout << getTimeStamp() << LOG_PREFIX_COLORED[m_LogType] << m_Logger.str() << std::endl;
  }
  writeErrorLog(Message);
}

std::string LOG::getTimeStamp()
{
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char buf[100] = {0};
  std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S", std::gmtime(&now)); // lgtm [cpp/potentially-dangerous-function]
  return buf;
}

void LOG::writeErrorLog(const std::string &errorMessage)
{
  string errfname = SDL_GetBasePath() + string{"error.log"};

  /* First we create the file if it doesn't exist */
  std::fstream fs(errfname, std::fstream::out | std::fstream::app);
  if (!fs)
    throw CytopiaError(TRACE_INFO "Could not open file " + errfname);

  /* We send the message */
  fs << errorMessage << std::endl;

  /* We compute the size of the file */
  fs.seekp(0, fs.end);
  std::streampos Size = fs.tellp();
  if (Size > MAX_LOG_SIZE_BYTES::value)
  {
    /* We need to rotate the logs */
    std::fstream fs(errfname, std::fstream::in | std::fstream::out);
    fs.seekg(0);
    string line;
    std::streampos Cut = 0;
    while (Size - Cut > MAX_LOG_SIZE_BYTES::value / 2 && std::getline(fs, line))
      Cut += line.size() + 1;
    stringstream truncatedstream;
    truncatedstream << fs.rdbuf();
    fs.close();
    fs.open(SDL_GetBasePath() + string{"error.log"}, std::fstream::trunc | std::fstream::out);
    fs << truncatedstream.str();
  }
}
