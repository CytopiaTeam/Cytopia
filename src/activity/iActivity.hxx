#ifndef I_ACTIVITY_HXX
#define I_ACTIVITY_HXX

#include <memory>
#include <betterEnums.hxx>

#include "../layout/iLayout.hxx"
#include "../view/iViewElement.hxx"
#include "../GameService.hxx"
#include "../model/iModel.hxx"
#include "../activity/ActivityType.hxx"
#include "../controller/iController.hxx"
#include "../services/MouseController.hxx"

class iActivity : public GameService, public virtual iView
{
  class Window & m_Window;
  std::vector<iModelPtr> m_States;
  std::vector<iControllerPtr> m_Controllers;
  friend class Window;

public:
 
  iActivity(GameService::ServiceTuple &, class Window &);
  virtual ~iActivity() = 0;

protected:

  /**
   * @brief Switches to another iActivity
   * @param ActivityType the type of iActivity to switch to
   */
  void activitySwitch(ActivityType);

  /**
   * @brief Creates a new iModel
   * @tparam StateType the type of the state
   * @tparam Args arguments to construct the state
   * @returns a reference to the created state
   */  
  template <typename StateType, typename... Args>
  StateType & createState(Args &&... args);
  
  /**
   * @brief Creates a new iController
   * @tparam ControllerType the type of the controller
   * @tparam Args arguments to construct the controller
   * @returns a referenece to the created controller
   */  
  template <typename ControllerType, typename... Args>
  ControllerType & createController(Args &&... args);

  /**
   * @returns the Window this Activity is assigned to 
   */
  Window & getWindow() noexcept;
};

using iActivityPtr = std::unique_ptr<iActivity>;

#include "iActivity.inl.hxx"

#endif // I_ACTIVITY_HXX
