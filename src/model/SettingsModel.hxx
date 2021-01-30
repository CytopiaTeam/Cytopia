#ifndef SETTINGS_MODEL_HXX_
#define SETTINGS_MODEL_HXX_

#include "DataModel.hxx"
#include "Localization.hxx"

struct LanguageChangeEvent
{
  const iLocalization &localization;
};

using SettingsModelNotification = TypeList<LanguageChangeEvent>;

class SettingsModel : public DataModel<SettingsModelNotification>
{
public:
  SettingsModel(UILoopMQ &mq);
  const iLocalization &getLocalization() { return m_Localization; }
  void setLanguage(std::string language);
  std::string getLanguage() { return m_Language; }

private:
  std::string m_Language;
  Localization m_Localization;
};

using SettingsModelListener = SettingsModel::Listener;

#endif // SETTINGS_MODEL_HXX_