#include "button.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), rect(uiElementRect)
{

}

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
}
