#ifndef ACTIVITY_TYPE_HXX
#define ACTIVITY_TYPE_HXX

#include <betterEnums.hxx>

BETTER_ENUM(ActivityType, uint8_t, MainMenu, NewGame, LoadGame, LanguageSelection, PluginSelection);
using ActivityImplementations = TypeList<
  class MainMenuActivity,
  class NewGameActivity,
  class LoadGameActivity,
  class LanguageSelectionActivity,
  class PluginSelectionActivity
>;

#endif // ACTIVITY_TYPE_HXX
