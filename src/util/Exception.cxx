#include "Exception.hxx"


#ifdef _WIN32
/**
 *  @todo implement window stack trace
 */
#else

#include <unistd.h>
#include <execinfo.h>
#include <signal.h>

void SIG_handler(int signal)
{
  switch(signal) {
    case SIGINT: [[fallthrough]];
    case SIGTERM:
      {
        LOG(LOG_INFO) << "Abort signal received";
        exit(0);
      }
    case SIGSEGV:
      {
        LOG(LOG_ERROR) << "Segmentation Fault";
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

#endif

SDL_AssertState AssertionHandler(const SDL_AssertData *data, void *)
{
  LOG(LOG_ERROR) << "SDL2 Assertion failure";

  #ifdef _WIN32
  /**
   * @todo print windows traceback
   */
  #else
  /* We print the last 10 calls */
  void* buffer[10];
  size_t size;
  size = backtrace(buffer, 10);
  backtrace_symbols_fd(buffer, size, STDERR_FILENO);
  #endif

  return SDL_ASSERTION_ABORT;
}


