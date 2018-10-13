#include "button.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), rect(uiElementRect) {}

void Button::draw()
{

  if (rect.h != 0 && rect.w != 0)
  {
    drawButtonFrame(rect);
  }

  // render the buttons texture if available
  renderTexture();
}

void Button::onMouseButtonUp(SDL_Event &event)
{
  clickSignal.emit();

  if (getParentID() != "")
  {
    toggleGroupSignal.emit(getParentID());
  }

  changeButtonState(TextureManager::ButtonState::DEFAULT);
}

void Button::onMouseButtonDown(SDL_Event &event) { changeButtonState(TextureManager::ButtonState::CLICKED); }

void Button::onMouseEnter(SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(TextureManager::ButtonState::CLICKED);
  }
  else
  {
    changeButtonState(TextureManager::ButtonState::HOVERING);
  }
}

void Button::onMouseLeave(SDL_Event &event) { changeButtonState(TextureManager::ButtonState::DEFAULT); }

void Button::registerFunction(std::function<void()> const &cb) { clickSignal.connect(cb); }
void Button::registerToggleUIFunction(std::function<void(const std::string &)> const &cb) { toggleGroupSignal.connect(cb); }
