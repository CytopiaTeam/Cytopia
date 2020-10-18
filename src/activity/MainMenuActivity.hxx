#ifndef MAIN_MENU_ACTIVITY_HXX
#define MAIN_MENU_ACTIVITY_HXX

#include <functional>

#include "../layout/AbsoluteLayout.hxx"
#include "iActivity.hxx"
#include "../util/LOG.hxx"

class MainMenuActivity : public AbsoluteLayout, public iActivity
{
  void onNewGame();
  void onLoadGame();
  void onExit();
  void onLanguageSelection();
  void onPluginSelection();
public:
  MainMenuActivity(GameService::ServiceTuple &, class Window &);
  virtual ~MainMenuActivity() final;
  void setup(class GameService & constext) noexcept final;
};

#endif // MAIN_MENU_ACTIVITY_HXX
