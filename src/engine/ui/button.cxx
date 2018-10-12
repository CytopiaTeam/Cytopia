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

void Button::onMouseButtonUp()
{
  clickSignal.emit();
  LOG() << "Click event from button class itself was triggered";

  if (getParentID() != "")
  {
    toggleGroupSignal.emit(getParentID());
  }
}

void Button::registerFunction(std::function<void()> &cb) { clickSignal.connect(cb); }
void Button::registerToggleUIFunction(std::function<void(const std::string &)> &cb) { toggleGroupSignal.connect(cb); }
