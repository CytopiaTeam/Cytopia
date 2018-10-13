#include "checkbox.hxx"

Checkbox::Checkbox(const SDL_Rect &uiElementRect) : UiElement(SDL_Rect{uiElementRect.x, uiElementRect.y, 20, 20})
{
  rect = uiElementRect;
  rect.w = 20;
  rect.h = 20;
  // checkbox is always a togglebutton
  setToggleButton(true);
}

void Checkbox::draw()
{
  drawButtonFrame(rect);

  if (getButtonState() == TextureManager::TOGGLED)
  {
    drawSolidRect(SDL_Rect{(rect.x + 7), rect.y + 7, rect.w - 13, rect.h - 13}, SDL_Color{84, 84, 84});
  }
}

void Checkbox::onMouseButtonUp(SDL_Event &event)
{
  clickSignal.emit();

  if (getParentID() != "")
  {
    toggleGroupSignal.emit(getParentID());
  }

  changeButtonState(checked ? TextureManager::ButtonState::DEFAULT : TextureManager::ButtonState::TOGGLED);
  checked = !checked;
}

void Checkbox::onMouseButtonDown(SDL_Event &event)
{
  changeButtonState(checked ? TextureManager::ButtonState::DEFAULT : TextureManager::ButtonState::TOGGLED);
}

void Checkbox::onMouseEnter(SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(checked ? TextureManager::ButtonState::DEFAULT : TextureManager::ButtonState::TOGGLED);
  }
}

void Checkbox::onMouseLeave(SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(checked ? TextureManager::ButtonState::TOGGLED : TextureManager::ButtonState::DEFAULT);
  }
}
