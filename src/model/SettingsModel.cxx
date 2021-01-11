#include "SettingsModel.hxx"
#include "Settings.hxx"

SettingsModel::SettingsModel(UILoopMQ &mq)
    : DataModel<SettingsModelNotification>{mq}, m_Language{Settings::instance().gameLanguage}, m_Localization{}, m_UiQueue{mq}
{
  m_Localization.load(m_Language);
}

void SettingsModel::setLanguage(std::string language)
{
  if (language != m_Language)
  {
    std::swap(m_Language, language);
    m_Localization.load(m_Language);
    notifyObservers(Notification{LanguageChangeEvent{m_Localization}});
    // TODO: this should be removed once notification is reworked and cause UI to be redrawn
    m_UiQueue.push(WindowRedrawEvent{});
  }
}
