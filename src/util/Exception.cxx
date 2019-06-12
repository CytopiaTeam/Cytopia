#include "Exception.hxx"


#ifdef __WIN__
/**
 *  @todo implement window stack trace
 */
#else

#include <unistd.h>

void SIG_handler(int signal)
{
  switch(signal) {
    case SIGINT: [[fallthrough]]
    case SIGTERM:
      {
        LOG(LOG_INFO) << "Abort signal received";
        exit(0);
      }
    case SIGSEGV:
      {
        LOG(LOG_EXCEPTION) << "Segmentation Fault";
        break;
      }
    default:
      {
        LOG(LOG_ERROR) << "Unknown signal received";
        break;
      }
  }
  /* We print the last 10 calls */
  void* buffer[10];
  size_t size;
  size = backtrace(buffer, 10);
  backtrace_symbols_fd(buffer, size, STDERR_FILENO);
  exit(1);
}

SDL_AssertState AssertionHandler(const SDL_AssertData *data, void *)
{
  LOG(LOG_EXCEPTION) << "SDL2 Assertion failure";
  /* We print the last 10 calls */
  void* buffer[10];
  size_t size;
  size = backtrace(buffer, 10);
  backtrace_symbols_fd(buffer, size, STDERR_FILENO);
  return SDL_ASSERTION_ABORT;
}

#endif
