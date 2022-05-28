#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#define CYTOPIA_PLATFORM_WIN
#define CYTOPIA_PLATFORM_WINPC
constexpr const char *CYTOPIA_PLATFORM_NAME = "win";
#elif defined(__APPLE_CC__) || defined(__APPLE__) || defined(__APPLE_CPP__) || defined(__MACOS_CLASSIC__)
#define CYTOPIA_PLATFORM_UNIX
#define CYTOPIA_PLATFORM_MACOSX
constexpr const char *CYTOPIA_PLATFORM_NAME = "macosx";
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
#define CYTOPIA_PLATFORM_UNIX
#define CYTOPIA_PLATFORM_XBSD
constexpr const char *CYTOPIA_PLATFORM_NAME = "freebsd";
#elif defined(__HAIKU__) || defined(HAIKU)
#define CYTOPIA_PLATFORM_BEOS
#define CYTOPIA_PLATFORM_HAIKU
constexpr const char *CYTOPIA_PLATFORM_NAME = "haiku";
#elif defined(ANDROID)
#define CYTOPIA_PLATFORM_UNIX
#define CYTOPIA_PLATFORM_ANDROID
constexpr const char *CYTOPIA_PLATFORM_NAME = "android";
#else
#define CYTOPIA_PLATFORM_UNIX
#define CYTOPIA_PLATFORM_LINUX
constexpr const char *CYTOPIA_PLATFORM_NAME = "linux";
#endif

#endif // PLATFORM_H_