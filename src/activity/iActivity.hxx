#ifndef I_ACTIVITY_HXX
#define I_ACTIVITY_HXX

#include <memory>
#include <vector>
#include <betterEnums.hxx>

#include "../GameService.hxx"
#include "../model/iModel.hxx"
#include "../view/iView.hxx"

BETTER_ENUM(ActivityType, uint8_t, MainMenu, NewGame, LoadGame, LanguageSelection, PluginSelection);

using ActivityImplementations = TypeList<
  class MainMenuActivity,
  class NewGameActivity,
  class LoadGameActivity,
  class LanguageSelectionActivity,
  class PluginSelectionActivity
  /* Add your Activity Type here */
>;

class Window;

class iActivity : public GameService, public virtual iView
{
  Window & m_Window;
  std::vector<iModelPtr> m_States;
  friend class Window;

public:
 
  iActivity(GameService::ServiceTuple &, Window &);
  virtual ~iActivity() = 0;

protected:

  /**
   * @brief Switches to another iActivity
   * @param ActivityType the type of iActivity to switch to
   */
  void activitySwitch(ActivityType);

  /**
   * @brief Creates a new State
   * @tparam StateType the type of the state
   * @tparam Args arguments to construct the state
   * @returns a referenece to the created state
   */  
  template <typename StateType, typename... Args>
  StateType & createState(Args &&... args);

  /**
   * @returns the Window this Activity is assigned to 
   */
  Window & getWindow() noexcept;
};

using iActivityPtr = std::unique_ptr<iActivity>;

#include "iActivity.inl.hxx"

#endif // I_ACTIVITY_HXX
