#ifndef EXCEPTION_HXX_
#define EXCEPTION_HXX_

#include <stdexcept>
#include <exception>

#define ERROR_MSG "In file " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"
#define NESTED_MSG "Called from " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"
#define STRINGIFY(x) #x
#define STRINGIFY2(x) STRINGIFY(x)
#define __line__ STRINGIFY2(__LINE__)
#define TRACE_INFO "Exception thrown from " + string(__PRETTY_FUNCTION__) + " at " __FILE__ ":" __line__ " - "

// Required to use std::getenv without warning on MSVC
#define _CRT_SECURE_NO_WARNINGS

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif __INTEL_COMPILER
#define __PRETTY_FUNCTION__ "Unknown_Method"
#endif

using RuntimeError = std::runtime_error;

/**
 * @brief A generic error in Cytopia
 */
class CytopiaError : public RuntimeError
{
  using RuntimeError::RuntimeError;
};

/**
 * @brief A configuration error
 */
class ConfigurationError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief An unimplemented function was called
 */
class UnimplementedError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief An audio-related error occured
 */
class AudioError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief A font-related error occured
 */
class FontError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief A display error occured
 */
class DisplayError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief A UI-related error occured
 */
class UIError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief A compression error occured
 */
class CompressionError : public CytopiaError
{
  using CytopiaError::CytopiaError;
};

/**
 * @brief install OS crash handler
 */
void systemSetupCrashHandler();

#endif
