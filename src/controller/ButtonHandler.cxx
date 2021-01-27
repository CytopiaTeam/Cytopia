#include "ButtonHandler.hxx"
#include "../util/LOG.hxx"
#include "../events/MouseEvents.hxx"
#include "../model/MouseState.hxx"
#include "../util/Rectangle.hxx"
#include "../services/MouseController.hxx"

ButtonHandler::ButtonHandler(Callback callback, ButtonModel &state, MouseState &mouse_state, const iView &view)
    : m_HandleClick(callback), m_State(state), m_MouseState(mouse_state), m_View(view), m_IsHovered(false)
{
}

ButtonHandler::~ButtonHandler() = default;

void ButtonHandler::onMouseHover()
{
  m_MouseState.setCursor(CursorType::Pointer);
  m_State.setStatus(ButtonStatus::Hovered);
  m_IsHovered = true;
}

void ButtonHandler::onMouseLeftButtonDown(ClickEvent &&event) { m_State.setStatus(ButtonStatus::Pressed); }

void ButtonHandler::onMouseLeftButtonUp(ClickEvent &&event)
{
  m_HandleClick();

  if (m_IsHovered)
  {
    m_State.setStatus(ButtonStatus::Hovered);
  }
}

void ButtonHandler::onMouseLeave()
{
  m_MouseState.setCursor(CursorType::Arrow);
  m_State.setStatus(ButtonStatus::Normal);
  m_IsHovered = false;
}

const iShape &ButtonHandler::getShape() { return m_View.getBounds(); }

void ButtonHandler::setup(GameService &context) noexcept
{
  auto &mouseCtrl = context.GetService<MouseController>();
  mouseCtrl.addHandler(this);
}
