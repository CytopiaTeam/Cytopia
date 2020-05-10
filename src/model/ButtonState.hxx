#ifndef BUTTON_STATE_HXX
#define BUTTON_STATE_HXX

#include <betterEnums.hxx>
#include "DataModel.hxx"

BETTER_ENUM(ButtonStatus, uint8_t, Normal, Disabled, Pressed, Hovered);

using ButtonNotifications = TypeList<ButtonStatus>;

class ButtonState : public DataModel<ButtonNotifications>
{
  ButtonStatus m_Status = ButtonStatus::Normal;
public:
  using DataModel::DataModel;
  void setStatus(ButtonStatus);
  ~ButtonState() final;
};

using ButtonListener = ButtonState::Listener;

#endif // BUTTON_STATE_HXX
