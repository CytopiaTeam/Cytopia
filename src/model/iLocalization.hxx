#ifndef I_LOCALIZATION_HXX_
#define I_LOCALIZATION_HXX_

#include <string>

/**
* Interface to the localization model.
*/
class iLocalization
{
public:
  virtual std::string translateText(const std::string &text) const = 0;
  virtual std::string translateText(const char *text) const = 0;
  virtual ~iLocalization(){};
};

#endif // I_LOCALIZATION_HXX_