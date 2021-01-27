#ifndef BUTTON_CONTROLLER_HXX
#define BUTTON_CONTROLLER_HXX

#include <functional>

#include "iMouseHandler.hxx"
#include "../model/ButtonModel.hxx"
#include "../model/MouseState.hxx"
#include "../util/iShape.hxx"
#include "../view/iView.hxx"

class ButtonHandler : public iMouseHandler
{
public:
  using Callback = std::function<void()>;
  ButtonHandler(Callback, ButtonModel &, MouseState &, const iView &);
  ~ButtonHandler() override;
  void onMouseLeftButtonDown(class ClickEvent &&) override;
  void onMouseLeftButtonUp(class ClickEvent &&) override;
  void onMouseHover() override;
  void onMouseLeave() override;
  const iShape &getShape() final;
  void setup(GameService &) noexcept override;

private:
  ButtonModel &m_State;
  MouseState &m_MouseState;
  Callback m_HandleClick;
  const iView &m_View;
  bool m_IsHovered;
};

#endif // BUTTON_CONTROLLER_HXX
