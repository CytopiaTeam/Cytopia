#include "Exception.hxx"
#include "LOG.hxx"

#ifdef _WIN32
#include <Windows.h>
#include <dbghelp.h>
#include <signal.h>

void SIG_handler(int signal)
{
  switch (signal)
  {
  case SIGINT:
    [[fallthrough]];
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
  void *buffer[10];
  HANDLE process = GetCurrentProcess();
  SymInitialize(process, nullptr, true);
  WORD size = CaptureStackBackTrace(0, 10, buffer, nullptr);
  SYMBOL_INFO symbol;
  {
    LOG logger(LOG_ERROR);
    for (int i = 0; i < size; ++i)
      logger << "\tat " << symbol.Name << "\n";
  }
  exit(1);
}

#elif __unix__ && !__ANDROID__ && !__EMSCRIPTEN__

#include <unistd.h>
#include <execinfo.h>
#include <signal.h>

void SIG_handler(int signal)
{
  switch (signal)
  {
  case SIGINT:
    [[fallthrough]];
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
  void *buffer[10];
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
  /* We print the last 10 calls */
  void *buffer[10];
  HANDLE process = GetCurrentProcess();
  SymInitialize(process, nullptr, true);
  WORD size = CaptureStackBackTrace(0, 10, buffer, nullptr);
  SYMBOL_INFO symbol;
  for (int i = 0; i < size; ++i)
    std::cout << "\tat " << symbol.Name << "\n";
#elif defined (__unix__) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
  /* We print the last 10 calls */
  void *buffer[10];
  size_t size;
  size = backtrace(buffer, 10);
  backtrace_symbols_fd(buffer, size, STDERR_FILENO);
#endif

  return SDL_ASSERTION_ABORT;
}
