#include "OSystem.hxx"
#include "LOG.hxx"
#include "platform.hxx"

#include <ctime>

#if defined(CYTOPIA_PLATFORM_LINUX) || defined(CYTOPIA_PLATFORM_HAIKU)
  #include <limits.h>
  #include <unistd.h>
  #include <sys/stat.h>
#endif

#ifdef CYTOPIA_PLATFORM_LINUX
#include <cstdlib>
#include <string.h>
const char* getDialogCommand()
{
  if (::system(NULL))
  {
    if(::system("which gdialog") == 0)
      return "gdialog";

    else if (::system("which kdialog") == 0)
      return "kdialog";
  }
  return NULL;
}
#elif defined(CYTOPIA_PLATFORM_MACOSX)
  #include <cstdlib>
#elif defined(CYTOPIA_PLATFORM_WIN)
  #include <windows.h>
#endif

void OSystem::error(const std::string& title, const std::string& text)
{
#if defined(CYTOPIA_PLATFORM_LINUX)
  const char * dialogCommand = getDialogCommand();
  if (dialogCommand)
  {
    std::string command = dialogCommand;
    command += " --title \"" + title + "\" --msgbox \"" + text + "\"";
    int syserror = ::system(command.c_str());
    if( syserror )
    {
      LOG(LOG_DEBUG) << "WARNING: Cant execute command " << command;
    }
  }

  // fail-safe method here, using stdio perhaps, depends on your application
#elif defined(CYTOPIA_PLATFORM_WIN)
  MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#endif
}

void OSystem::openUrl(const std::string& url, const std::string& prefix)
{
#ifdef CYTOPIA_PLATFORM_LINUX
  std::string command = prefix + "xdg-open '" + url + "'";
  LOG(LOG_DEBUG) << command;
  ::system( command.c_str() );
#elif defined(CYTOPIA_PLATFORM_WIN)
  ShellExecuteA(0, "Open", url.c_str(), 0, 0 , SW_SHOW);
#elif defined(CYTOPIA_PLATFORM_MACOSX)
  std::string command = "open \"" + url + "\" &";
  ::system(command.c_str());
#endif
}

void OSystem::openDir(const std::string& path, const std::string& prefix)
{
  std::string command;
#ifdef CYTOPIA_PLATFORM_LINUX
  command = prefix + "nautilus '" + path + "' &";
  ::system(command.c_str() );
#elif defined(CYTOPIA_PLATFORM_WIN)
  ShellExecute(GetDesktopWindow(), "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(CYTOPIA_PLATFORM_MACOSX)
  command = "open \"" + path + "\" &";
  ::system(command.c_str());
#endif
}

bool OSystem::is(OSystem::Type type)
{
#define RETURN_TRUE(t) if (t) return true;

#ifdef CYTOPIA_PLATFORM_WIN
  RETURN_TRUE(windows)
#endif

#ifdef CYTOPIA_PLATFORM_LINUX
  RETURN_TRUE(linux)
#endif

#ifdef CYTOPIA_PLATFORM_UNIX
  RETURN_TRUE(unix)
#endif

#ifdef CYTOPIA_PLATFORM_ANDROID
  RETURN_TRUE(android)
#endif

#ifdef CYTOPIA_PLATFORM_MACOSX
  RETURN_TRUE(macos)
#endif

#ifdef CYTOPIA_PLATFORM_XBSD
  RETURN_TRUE(bsd)
#endif

#ifdef CYTOPIA_PLATFORM_HAIKU
  RETURN_TRUE(haiku)
#endif

#ifdef CYTOPIA_PLATFORM_BEOS
  RETURN_TRUE(beos)
#endif

  return false;
}

bool OSystem::isAndroid() { return is( android ); }
bool OSystem::isLinux() { return is( linux ); }
bool OSystem::isUnix() { return is( unix ); }
bool OSystem::isMac() { return is( macos ); }
bool OSystem::isWindows() { return is( windows ); }