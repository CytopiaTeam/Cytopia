#include "buttonText.hxx"

ButtonText::ButtonText(int x, int y, int w, int h) : UiElement(x, y, w, h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawSolidRect(rect, SDL_Color{ 128, 128, 128 });
  addLabel("test button");
}

ButtonText::ButtonText(int x, int y, int w, int h, int groupID) : UiElement(x, y, w, h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawSolidRect(rect, SDL_Color{ 128, 128, 128 });
  addLabel("test button");
  setGroupID(groupID);
}

void ButtonText::addLabel(const std::string& text)
{
  drawText(text, SDL_Color{ 255,255,255 });
}