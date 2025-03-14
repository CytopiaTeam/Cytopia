#ifndef CYTOPIA_PARSECLI_HXX
#define CYTOPIA_PARSECLI_HXX

#include "SimpleOpt.h"
#include <string>

class ParseCli
{
public:
  bool ProcessCommandLine(int argc, char **argv);

private:
  std::string GetLastErrorText(int a_nError);

  void ShowUsage();
};

#endif //CYTOPIA_PARSECLI_HXX
