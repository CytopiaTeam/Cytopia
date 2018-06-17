#include "buttonText.hxx"

ButtonText::ButtonText(int x, int y, int w, int h, int groupID, int actionID, int parentOfGroup) : UiElement(x, y, w, h, groupID, actionID, parentOfGroup)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawSolidRect(rect, SDL_Color{ 128, 128, 128 });
  addLabel("test button");
}

void ButtonText::addLabel(const std::string& text)
{
  drawText(text, SDL_Color{ 255,255,255 });
}