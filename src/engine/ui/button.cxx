#include "button.hxx"



Button::Button(int x, int y, int w, int h, const std::string& text, int groupID, int actionID, int parentOfGroup) : UiElement(x, y, w, h, groupID, actionID, parentOfGroup)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawSolidRect(rect, SDL_Color{ 128, 128, 128 });
  drawText(text, SDL_Color{ 255,255,255 });
}