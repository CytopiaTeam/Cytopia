#include "AppOptions.hxx"

// build a vector of argv options
// the program path argv[0] is excluded
static std::vector<std::string > argvOptions;

void AppOptions::setArgvOptions(int argc, char **argv)
{
  argvOptions.clear();
  for (int i = 1; i < argc; i++)
    argvOptions.emplace_back(std::string(argv[i]));
}

bool AppOptions::parseOptions(const std::string &errorInfo)
{
  std::string arg, subArg, subArg2;

  for (auto &arg : argvOptions)
  {
    if (arg.compare(0, 2, "--") == 0)
    {
      parseOption(arg.substr(2), errorInfo);
    }
    else if (arg[0] == '-')
    {
      size_t i;
      for (i = 1; i < arg.length(); ++i)
      {
        if (i > 1 && isalpha((unsigned char)arg[i]) && arg[i - 1] != 'x')
        {
          // parse the previous option in subArg
          parseOption(subArg, errorInfo);
          subArg = "";
        }

        if (arg[i] == '-')
        {
          parseOption(subArg2, errorInfo);
          subArg2 = "";
          i++;
        }

        // append the current option to subArg
        subArg.append(1, arg[i]);
        subArg2.append(1, arg[i]);
      }

      subArg = "";
    }
    else
    {
      parseOption(arg, errorInfo);
      subArg = "";
    }
  }

  if (errorInfo.length() > 0)
    return false;

  return true;
}

bool AppOptions::isOption(const std::string &arg, const char *op)
{
  return arg.compare(op) == 0;
}

bool AppOptions::isOption(const std::string &arg, const char *a, const char *b)
{
  return (isOption(arg, a) || isOption(arg, b));
}

bool AppOptions::isParamOption(const std::string &arg, const char *option)
{
  bool retVal = arg.compare(0, strlen(option), option) == 0;
  // if comparing for short option, 2nd char of arg must be numeric
  if (retVal && strlen(option) == 1 && arg.length() > 1)
    if (!isdigit((unsigned char)arg[1]))
      retVal = false;
  return retVal;
}

bool AppOptions::isParamOption(const std::string &arg, const char *option1, const char *option2)
{
  return isParamOption(arg, option1) || isParamOption(arg, option2);
}

std::string AppOptions::getParam(const std::string &arg, const char *op)
{
  std::string param = arg.substr(strlen(op));
  if (param.size() > 1 && param.front() == '=')
    param.erase(0, 1);
  return param;
}

std::string AppOptions::getParam(const std::string &arg, const char *op1, const char *op2)
{
  return isParamOption(arg, op1) ? getParam(arg, op1) : getParam(arg, op2);
}

std::vector<std::string > &AppOptions::getArgvOptions()
{
  return argvOptions;
}

void AppOptions::parseOption(const std::string &arg, const std::string &errorInfo)
{
  // sample how to check options
  if (isParamOption(arg, "s", "map-size="))
  {
    std::string mapSizeStr = getParam(arg, "s", "map-size=");
    int mapSize = 0;
    if (mapSizeStr.length() > 0)
      mapSize = atoi(mapSizeStr.c_str());

    return;
  }
}