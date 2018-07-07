#include "text.hxx"

Text::Text(const SDL_Rect &uiElementRect, const std::string &text) : UiElement(uiElementRect, text) 
{
  drawText(text, SDL_Color{ 255,255,255 });
}