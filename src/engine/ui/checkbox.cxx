#include "checkbox.hxx"

Checkbox::Checkbox(const SDL_Rect &uiElementRect) : UiElement(SDL_Rect{uiElementRect.x, uiElementRect.y, 20, 20})
{
  // checkbox is always a togglebutton
  setToggleButton(true);
}

void Checkbox::draw()
{
  drawButtonFrame(_uiElementRect);

  if (getButtonState() == BUTTONSTATE_CLICKED)
  {
    drawSolidRect(SDL_Rect{(_uiElementRect.x + 7), _uiElementRect.y + 7, _uiElementRect.w - 13, _uiElementRect.h - 13},
                  SDL_Color{84, 84, 84});
  }
}

void Checkbox::onMouseButtonUp(const SDL_Event &event)
{
  clickSignal.emit();

  if (getParentID() != "")
  {
    toggleGroupSignal.emit(getParentID());
  }

  if (!_isMouseButtonDown)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
  }
  _isMouseButtonDown = false;
}

void Checkbox::onMouseButtonDown(const SDL_Event &event)
{
  changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
  _isMouseButtonDown = true;
}

void Checkbox::onMouseEnter(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    _isMouseButtonDown = true;
  }
}

void Checkbox::onMouseLeave(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_DEFAULT ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
  }
}
