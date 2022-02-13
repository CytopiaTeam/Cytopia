#ifndef SETTINGS_ACTIVITY_HXX
#define SETTINGS_ACTIVITY_HXX

#include "../layout/AbsoluteLayout.hxx"
#include "../activity/iActivity.hxx"
#include <vector>

class SettingsActivity : public AbsoluteLayout, public iActivity
{
private:
  void onBack();
  void onLanguageSelected(int index);
  int getSelectedLanguageIndex();

  struct LanguageSelection
  {
    std::string name;
    std::string shortName;
    bool isTranslated;
  };

  std::vector<LanguageSelection> m_Languages;

public:
  SettingsActivity(GameService::ServiceTuple &, class Window &);
  virtual ~SettingsActivity() final;
  void setup(class GameService &context) noexcept final;
};

#endif // LANGUAGE_SELECTION_ACTIVITY_HXX
