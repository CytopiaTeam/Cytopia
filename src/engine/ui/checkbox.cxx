#include "checkbox.hxx"

Checkbox::Checkbox(int x, int y, const std::string &groupID) : UiElement(x, y, 20, 20, groupID, 0, "")
{
  rect.x = x;
  rect.y = y;
  rect.w = 20;
  rect.h = 20;
  // checkbox is always a togglebutton
  setToggleButton(true);
}

void Checkbox::draw()
{
  Uint8 bgColor, bgColorFrame, bgColorFrameShade, bgColorBottomFrame, bgColorBottomFrameShade;

  if (getButtonState() == TextureManager::TOGGLED)
  {
    bgColor = 128;
    bgColorFrame = 106;
    bgColorFrameShade = 84;
    bgColorBottomFrame = 150;
    bgColorBottomFrameShade = 172;
  }
  else if (getButtonState() == TextureManager::HOVERING)
  {
    bgColor = 228;
    bgColorFrame = 250;
    bgColorFrameShade = 255;
    bgColorBottomFrame = 206;
    bgColorBottomFrameShade = 184;
  }
  else
  {
    bgColor = 128;
    bgColorFrame = 150;
    bgColorFrameShade = 172;
    bgColorBottomFrame = 106;
    bgColorBottomFrameShade = 84;
  }

  SDL_Color color = {128, 128, 128};
  SDL_Color color_temp = {128, 128, 128};

  // top frame
  drawSolidRect(rect, SDL_Color{bgColorFrameShade, bgColorFrameShade, bgColorFrameShade});
  drawSolidRect(SDL_Rect{rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4}, SDL_Color{bgColorFrame, bgColorFrame, bgColorFrame});
  // background
  drawSolidRect(SDL_Rect{rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8}, SDL_Color{bgColor, bgColor, bgColor});
  // bottom frame
  drawSolidRect(SDL_Rect{rect.x + 4, (rect.y + rect.h) - 4, rect.w - 4, 4},
                SDL_Color{bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame});
  drawSolidRect(SDL_Rect{(rect.x + rect.w) - 4, rect.y + 4, 4, rect.h - 4},
                SDL_Color{bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame});
  // bottom frame shade
  drawSolidRect(SDL_Rect{rect.x + 2, (rect.y + rect.h) - 2, rect.w - 2, 2},
                SDL_Color{bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade});
  drawSolidRect(SDL_Rect{(rect.x + rect.w) - 2, rect.y + 2, 2, rect.h - 2},
                SDL_Color{bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade});

  if (getButtonState() == TextureManager::TOGGLED)
  {
    drawSolidRect(SDL_Rect{(rect.x + 7), rect.y + 7, rect.w - 13, rect.h - 13}, SDL_Color{84, 84, 84});
  }
}