#include "ButtonHandler.hxx"
#include "../util/LOG.hxx"
#include "../events/MouseEvents.hxx"
#include "../model/MouseState.hxx"
#include "../util/Rectangle.hxx"

ButtonHandler::ButtonHandler(Callback callback, ButtonState & state, MouseState & mouse_state, const iView & view) :
  m_HandleClick(callback),
  m_State(state),
  m_MouseState(mouse_state),
  m_View(view)
{ }

ButtonHandler::~ButtonHandler() = default;

void ButtonHandler::onMouseHover()
{
  m_MouseState.setCursor(CursorType::Pointer);
  m_State.setStatus(ButtonStatus::Hovered);
}

void ButtonHandler::onMouseClick(ClickEvent && event)
{
  if(event.state & ClickEvent::Pressed && event.state & ClickEvent::Left)
    m_State.setStatus(ButtonStatus::Pressed);
  if(event.state & ClickEvent::Released && event.state & ClickEvent::Left)
  {
    m_State.setStatus(ButtonStatus::Hovered);
    m_HandleClick();
  }
}

void ButtonHandler::onMouseLeave()
{
  m_MouseState.setCursor(CursorType::Arrow);
  m_State.setStatus(ButtonStatus::Normal);
}

const iShape & ButtonHandler::getShape()
{
  return m_View.getBounds();
}
