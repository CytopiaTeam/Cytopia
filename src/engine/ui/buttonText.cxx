#include "buttonText.hxx"

ButtonText::ButtonText(int x, int y, int w, int h) : UiElement(x, y, w, h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawSolidRect(rect);
  addLabel("test button");
}

void ButtonText::addLabel(const std::string& text)
{
  drawText(text, SDL_Color(SDL_Color{ 255,255,255 }));
}