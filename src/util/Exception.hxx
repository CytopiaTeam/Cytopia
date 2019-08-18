#ifndef EXCEPTION_HXX_
#define EXCEPTION_HXX_

#include <stdexcept>
#include <exception>

#define ERROR_MSG "In file " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"
#define NESTED_MSG "Called from " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"

using RuntimeError = std::runtime_error;

class CytopiaError : public RuntimeError { using RuntimeError::RuntimeError; };
class ConfigurationError : public CytopiaError { using CytopiaError::CytopiaError; };
class UnimplementedError : public CytopiaError { using CytopiaError::CytopiaError; };
class AudioError : public CytopiaError { using CytopiaError::CytopiaError; };
class FontError : public CytopiaError { using CytopiaError::CytopiaError; };
class DisplayError : public CytopiaError { using CytopiaError::CytopiaError; };
class UIError : public CytopiaError { using CytopiaError::CytopiaError; };
class CompressionError : public CytopiaError { using CytopiaError::CytopiaError; };

#endif
