#include "../activity/MainMenuActivity.hxx"
#include "../activity/LanguageSelectionActivity.hxx"
#include "../activity/SettingsActivity.hxx"
/**
@todo Uncomment these activities once they are implemented
#include "../activity/NewGameActivity.hxx"
#include "../activity/LoadGameActivity.hxx"
*/

template <typename... Args>
iActivityPtr Window::fromActivityType(ActivityType type, Args&&... args)
{
  switch(type)
  {
    case ActivityType::MainMenu:
      return std::make_unique<MainMenuActivity>(std::forward<Args>(args)...);
    case ActivityType::LanguageSelection:
      return std::make_unique<LanguageSelectionActivity>(std::forward<Args>(args)...);
    case ActivityType::Settings:
      return std::make_unique<SettingsActivity>(std::forward<Args>(args)...);
    /**
    @todo: Uncomment these cases once they are implemented 
    @todo: Add support for mods to add their own activities?
    case ActivityType::NewGame:
      return std::make_unique<NewGameActivity>(std::forward<Args>(args)...);
    case ActivityType::LoadGame:
      return std::make_unique<LoadGameActivity>(std::forward<Args>(args)...);
     */
    default:
      throw UnimplementedError{TRACE_INFO "No such ActivityType exists..."};
  }
}
