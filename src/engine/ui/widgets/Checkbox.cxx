#include "Checkbox.hxx"

Checkbox::Checkbox(const SDL_Rect &uiElementRect) : UIElement(SDL_Rect{uiElementRect.x, uiElementRect.y, 20, 20})
{
  // checkbox is always a toggleButton
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

bool Checkbox::onMouseButtonUp(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
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
    return true;
  }
  return false;
}

bool Checkbox::onMouseButtonDown(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    m_isMouseButtonDown = true;
    return true;
  }
  return false;
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
