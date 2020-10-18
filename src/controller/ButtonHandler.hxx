#ifndef BUTTON_CONTROLLER_HXX
#define BUTTON_CONTROLLER_HXX

#include <functional>

#include "iMouseHandler.hxx"
#include "../model/ButtonState.hxx"
#include "../model/MouseState.hxx"
#include "../util/iShape.hxx"
#include "../view/iView.hxx"

class ButtonHandler : public iMouseHandler
{
public:
  using Callback = std::function<void()>;
  ButtonHandler(Callback, ButtonState &, MouseState &, const iView &);
  ~ButtonHandler() override;
  void onMouseClick(class ClickEvent &&) override;
  void onMouseHover() override;
  void onMouseLeave() override;
  const iShape & getShape() final;
private:
  ButtonState & m_State;
  MouseState & m_MouseState;
  Callback m_HandleClick;  
  const iView & m_View;
};

#endif // BUTTON_CONTROLLER_HXX
