#include "checkbox.hxx"

Checkbox::Checkbox(const SDL_Rect &uiElementRect) : UiElement(SDL_Rect{uiElementRect.x, uiElementRect.y, 20, 20})
{
  // checkbox is always a togglebutton
  setToggleButton(true);
}

void Checkbox::draw()
{
  drawButtonFrame(m_uiElementRect);

  if (getButtonState() == BUTTONSTATE_CLICKED)
  {
    drawSolidRect(SDL_Rect{(m_uiElementRect.x + 7), m_uiElementRect.y + 7, m_uiElementRect.w - 13, m_uiElementRect.h - 13},
                  SDL_Color{84, 84, 84});
  }
}

void Checkbox::onMouseButtonUp(const SDL_Event &)
{
  clickSignal.emit();

  if (!elementData.actionParameter.empty())
  {
    toggleGroupSignal.emit(elementData.actionParameter);
  }

  if (!m_isMouseButtonDown)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
  }
  m_isMouseButtonDown = false;
}

void Checkbox::onMouseButtonDown(const SDL_Event &)
{
  changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
  m_isMouseButtonDown = true;
}

void Checkbox::onMouseEnter(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    m_isMouseButtonDown = true;
  }
}

void Checkbox::onMouseLeave(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_DEFAULT ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
  }
}
