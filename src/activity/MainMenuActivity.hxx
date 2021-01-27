#ifndef MAIN_MENU_ACTIVITY_HXX
#define MAIN_MENU_ACTIVITY_HXX

#include <functional>

#include "../layout/AbsoluteLayout.hxx"
#include "iActivity.hxx"
#include "../util/LOG.hxx"

class MainMenuActivity final : public AbsoluteLayout, public iActivity
{
  void onNewGame();
  void onLoadGame();
  void onExit();
  void onLanguageSelection();
  void onPluginSelection();

public:
  MainMenuActivity(GameService::ServiceTuple &, class Window &);
  virtual ~MainMenuActivity();
  void setup(class GameService &context) noexcept;
};

#endif // MAIN_MENU_ACTIVITY_HXX
