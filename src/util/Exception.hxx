#ifndef EXCEPTION_HXX_
#define EXCEPTION_HXX_

#include <stdexcept>
#include <exception>

#include "LOG.hxx"

#define ERROR_MSG "In file " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"
#define NESTED_MSG "Called from " __BOLD__ __RED__ __FILE__ ":" __line__ __CLEAR__ "\n\t"

using RuntimeError = std::runtime_error;

class ConfigurationError : public RuntimeError { using RuntimeError::RuntimeError; };
class UnimplementedError : public RuntimeError { using RuntimeError::RuntimeError; };
class MissingResourceError : public RuntimeError { using RuntimeError::RuntimeError; };

#endif
