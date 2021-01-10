#ifndef BUTTON_MODEL_HXX
#define BUTTON_MODEL_HXX

#include <betterEnums.hxx>
#include "DataModel.hxx"

BETTER_ENUM(ButtonStatus, uint8_t, Normal, Disabled, Pressed, Hovered);

using ButtonNotifications = TypeList<ButtonStatus>;

class ButtonModel : public DataModel<ButtonNotifications>
{
  ButtonStatus m_Status = ButtonStatus::Normal;

public:
  using DataModel::DataModel;
  void setStatus(ButtonStatus);
  ~ButtonModel() final;
};

using ButtonListener = ButtonModel::Listener;

#endif // BUTTON_MODEL_HXX
