#ifndef LOCALIZATION_HXX_
#define LOCALIZATION_HXX_

#include "moFileReader.hpp"
#include "iLocalization.hxx"

class Localization : public iLocalization
{
public:
  Localization();
  Localization(const Localization &) = delete;
  Localization(Localization &&) = delete;
  Localization &operator=(const Localization &) = delete;
  Localization &operator=(Localization &&) = delete;
  virtual ~Localization() = default;

  std::string translateText(const std::string &text) const override;
  std::string translateText(const char *text) const override;
  
  /**
  * @brief Load new language.
  * 
  * @param language String represents translated language. E.g. "nl"
  */
  void load(const std::string &language);

private:
  moFileLib::moFileReader m_MoFileReader;
  bool m_IsInitilaized;
};

#endif // LOCALIZATION_HXX_