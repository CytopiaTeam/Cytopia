#ifndef EXCEPTION_HXX_
#define EXCEPTION_HXX_

#include <stdexcept>
#include <exception>

#include "LOG.hxx"

#define ERROR_MSG "In file " BOLD RED __FILE__ ":" __line__ CLEAR "\n\t"
#define NESTED_MSG "Called from " BOLD RED __FILE__ ":" __line__ CLEAR "\n\t"

using RuntimeError = std::runtime_error;

class ConfigurationError : public RuntimeError { using RuntimeError::RuntimeError; };
class UnimplementedError : public RuntimeError { using RuntimeError::RuntimeError; };

void SIG_handler(int signal);

#ifdef __WIN__

#else
#include <execinfo.h>
#include <signal.h>
#endif

#endif
