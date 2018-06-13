#include "buttonText.hxx"

ButtonText::ButtonText(int x, int y, int w, int h) : UiElement(x, y, w, h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawSolidRect(rect);
}
