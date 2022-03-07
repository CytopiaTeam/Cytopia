#ifndef EXCEPTION_HXX_
#define EXCEPTION_HXX_

#include <stdexcept>
#include <exception>

#define ERROR_MSG "In file " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"
#define NESTED_MSG "Called from " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"

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

/*
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

#endif
