#ifndef OSYSTEM_H_
#define OSYSTEM_H_

#include <string>

struct OSystem
{
  enum class Type
  {
    unknown,
    windows,
    unix,
    linux,
    macos,
    bsd,
    haiku,
    beos,
    android
  };
  static void error(const std::string &title, const std::string &text);
  static void openUrl(const std::string &url, const std::string &prefix = "");
  static void openDir(const std::string &path, const std::string &prefix = "");

  static bool is(Type type);
  static bool isAndroid();
  static bool isLinux();
  static bool isUnix();
  static bool isMac();
  static bool isWindows();
};

#endif // OSYSTEM_H_