#include "Exception.hxx"
#include "LOG.hxx"

#include "SDL.h"

#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__OpenBSD__) && !defined(__vita__) && !defined(__SWITCH__) &&         \
    !defined(__ANDROID__) && !defined(__HAIKU__) && !defined(__EMSCRIPTEN__)

#include <execinfo.h>
#include <signal.h>

static void lwBacktracePrint(void)
{
  void *earray[100];
  int size = backtrace(earray, 100);

  char **stack = backtrace_symbols(earray, size);

  for (int i = 0; i < size; i++)
  {
    LOG(LOG_ERROR) << stack[i];
  }
}

static void lwCrashHandler(int sig)
{
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Oops, crashed with signal %d :(", sig);
  lwBacktracePrint();
  exit(1);
}

void systemSetupCrashHandler(void) { signal(SIGSEGV, lwCrashHandler); }

#elif defined(_WIN32) && defined(_M_X64)

#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

static const char *lwPrintExceptionName(DWORD exception_code)
{
  switch (exception_code)
  {
  case EXCEPTION_ACCESS_VIOLATION:
    return "Access violation";
  case EXCEPTION_DATATYPE_MISALIGNMENT:
    return "Datatype misalignment";
  case EXCEPTION_BREAKPOINT:
    return "Breakpoint";
  case EXCEPTION_SINGLE_STEP:
    return "Single step";
  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    return "Array bounds exceeded";
  case EXCEPTION_FLT_DENORMAL_OPERAND:
    return "Float denormal operand";
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    return "Float divide by zero";
  case EXCEPTION_FLT_INEXACT_RESULT:
    return "Float inexact result";
  case EXCEPTION_FLT_INVALID_OPERATION:
    return "Float invalid operation";
  case EXCEPTION_FLT_OVERFLOW:
    return "Float overflow";
  case EXCEPTION_FLT_STACK_CHECK:
    return "Float stack check";
  case EXCEPTION_FLT_UNDERFLOW:
    return "Float underflow";
  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    return "Integer divide by zero";
  case EXCEPTION_INT_OVERFLOW:
    return "Integer overflow";
  case EXCEPTION_PRIV_INSTRUCTION:
    return "Privileged instruction";
  case EXCEPTION_IN_PAGE_ERROR:
    return "In page error";
  case EXCEPTION_ILLEGAL_INSTRUCTION:
    return "Illegal instruction";
  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    return "Noncontinuable exception";
  case EXCEPTION_STACK_OVERFLOW:
    return "Stack overflow";
  case EXCEPTION_INVALID_DISPOSITION:
    return "Invalid disposition";
  case EXCEPTION_GUARD_PAGE:
    return "Guard page";
  case EXCEPTION_INVALID_HANDLE:
    return "Invalid handle";
  }
  return "Unknown exception";
}

static void lwSystemError(const char *title, const char *text) { MessageBox(NULL, text, title, MB_OK | MB_ICONERROR); }

// !!! NOTE !!!
// Please change this code with accuracy, it used native microsoft sample from
// https://social.msdn.microsoft.com/Forums/vstudio/en-US/f93a211a-9c95-42f0-8581-50314457b729/generating-the-stack-traces-in-the-c-code?forum=vsdebug

static void lwPrintStacktrace(LPEXCEPTION_POINTERS e)
{
  PIMAGEHLP_SYMBOL64 pSym;
  STACKFRAME sf;
  DWORD64 dwModBase, Disp64;
  DWORD Disp;
  BOOL more = FALSE;
  IMAGEHLP_LINE64 line;
  const char *filename = NULL;
  unsigned int linenum = 0;
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
  int count = 0;
  char modname[MAX_PATH];
  pSym = (PIMAGEHLP_SYMBOL64)GlobalAlloc(GMEM_FIXED, 16384);
  ZeroMemory(&sf, sizeof(sf));
  sf.AddrPC.Mode = AddrModeFlat;
  sf.AddrStack.Mode = AddrModeFlat;
  sf.AddrFrame.Mode = AddrModeFlat;
  sf.AddrReturn.Mode = AddrModeFlat;

  PCONTEXT context = e->ContextRecord;
  DWORD machine_type = 0;
  sf.AddrPC.Offset = context->Rip;
  sf.AddrFrame.Offset = context->Rbp;
  sf.AddrStack.Offset = context->Rsp;
  machine_type = IMAGE_FILE_MACHINE_AMD64;

  // Record exception info
  LOG(LOG_ERROR) << "Exception [" << lwPrintExceptionName(e->ExceptionRecord->ExceptionCode) << "] 0x" << std::hex
                 << (unsigned int)e->ExceptionRecord->ExceptionCode;
  LOG(LOG_ERROR) << "Exception Address: 0x" << std::hex << e->ExceptionRecord->ExceptionAddress;

  // Record stacktrace
  LOG(LOG_ERROR) << "Stacktrace:";

  while (1)
  {
    more = StackWalk(machine_type, GetCurrentProcess(), GetCurrentThread(), &sf, context, NULL, SymFunctionTableAccess64,
                     SymGetModuleBase64, NULL);
    if (!more || sf.AddrFrame.Offset == 0)
    {
      break;
    }
    dwModBase = SymGetModuleBase64(GetCurrentProcess(), sf.AddrPC.Offset);
    if (dwModBase)
    {
      GetModuleFileName((HINSTANCE)dwModBase, modname, MAX_PATH);
    }
    else
    {
      strcpy(modname, "Unknown");
    }

    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    pSym->MaxNameLength = MAX_PATH;

    // unwind callstack
    if (SymGetSymFromAddr64(GetCurrentProcess(), sf.AddrPC.Offset, &Disp64, pSym))
    {
      int filelineinfo_ok = SymGetLineFromAddr64(GetCurrentProcess(), sf.AddrPC.Offset, &Disp, &line);
      filename = !!filelineinfo_ok ? line.FileName : "UNKNOWN FILE";
      linenum = !!filelineinfo_ok ? line.LineNumber : 0;

      // This is the code path taken on VC if debugging syms are found
      LOG(LOG_ERROR) << count << " " << filename << ":" << linenum << " |" << pSym->Name << "+0x" << Disp << std::hex
                     << (unsigned int)sf.AddrPC.Offset;
    }
    else
    {
      // This is the code path taken on MinGW, and VC if no debugging syms are found.
      LOG(LOG_ERROR) << count << " " << modname << "+0x" << std::hex << (unsigned int)sf.AddrPC.Offset;
    }
    ++count;
  }
  GlobalFree(pSym);
}

/** Called by windows if an exception happens. */
static LONG CALLBACK lwExceptionHandler(LPEXCEPTION_POINTERS e)
{
  // Prologue.
  LOG(LOG_ERROR) << "Oops, crashed :(";

  // Initialize IMAGEHLP.DLL.
  SymInitialize(GetCurrentProcess(), ".", TRUE);

  lwPrintStacktrace(e);

  // Unintialize IMAGEHLP.DLL
  SymCleanup(GetCurrentProcess());

  // Inform user
  lwSystemError("Cytopia has crashed :(", "There was an unrecoverable error in Cytopia, which will now close.\n\n"
                                          "The piece of code that caused the crash has been saved to log.\n\n"
                                          "If you can, please create an issue by going to:\n\n"
                                          "https://github.com/CytopiaTeam/Cytopia/issues/new \n\n"
                                          "Please attatch log and your map to the issue report.\n\n"
                                          "With your help, we can avoid this crash in the future.\n\n"
                                          "Thanks!\n\n");

  // this seems to silently close the application
  return EXCEPTION_EXECUTE_HANDLER;
}

void systemSetupCrashHandler() { SetUnhandledExceptionFilter(lwExceptionHandler); }

#else // fallback

void systemSetupCrashHandler(void) {}

#endif
