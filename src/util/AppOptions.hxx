#ifndef APPOPTIONS_HXX_
#define APPOPTIONS_HXX_

#include <vector>
#include <string>

struct AppOptions {
  template<class Func>
  static bool parseOption(const char *a, const char *b, const Func &func)
  {
    bool hasb = (b && *b);
    for (auto &arg : getArgvOptions())
    {
      if (arg.compare(0, 2, "--") == 0)
      {
        std::string rarg = arg.substr(2);
        if (hasb && isParamOption(rarg, a, b)) {
          std::string param = getParam(rarg, a, b);
          return func(param);
        } 
        else if (isParamOption(rarg, a))
        {
          std::string param = getParam(rarg, a);
          return func(param);
        }
      }
      else if (arg.front() == '-')
      {
        std::string rarg = arg.substr(1);
        if (hasb && isParamOption(rarg, a, b)) {
          std::string param = getParam(rarg, a, b);
          return func(param);
        }
        else if (isParamOption(rarg, a))
        {
          std::string param = getParam(rarg, a);
          return func(param);
        }
      }
      else 
      {
        if (hasb && isOption(arg, a, b)) {
          return func("");
        }
        else if (isOption(arg, a)) {
          return func("");
        }
      }
    }
    return false;
  }

  static bool parseOption(const char *a, const char *b) { return parseOption(a, b, [] (auto &) { return true; }); }
  static std::string parseOption(const char *a, const char *b, const char *def) { 
    std::string param;
    bool found = parseOption(a, b, [&param] (auto &p) { param = p; return true; });
    return found ? param : def;
  }

  static void setArgvOptions(int argc, char **argv);
  static bool parseOptions(const std::string &errorInfo);

private:
  static void parseOption(const std::string &arg, const std::string &errorInfo);
  static bool isOption(const std::string &arg, const char *op);
  static bool isOption(const std::string &arg, const char *a, const char *b);
  static bool isParamOption(const std::string &arg, const char *option);
  static bool isParamOption(const std::string &arg, const char *option1, const char *option2);
  static std::string getParam(const std::string &arg, const char *op);
  static std::string getParam(const std::string &arg, const char *op1, const char *op2);
  static std::vector<std::string> &getArgvOptions();

  AppOptions(const AppOptions &) = delete;
  AppOptions &operator=(AppOptions &) = delete;
};
#endif // APPOPTIONS_HXX_
