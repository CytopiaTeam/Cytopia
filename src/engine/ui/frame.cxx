#include "frame.hxx"

Frame::Frame(int x, int y, int w, int h, int groupID, int actionID, int parentOfGroup)
    : UiElement(x, y, w, h, groupID, actionID, parentOfGroup)
{
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
}

void Frame::draw()
{
  // render the buttons texture if available

  Uint8 bgColor = 128;
  Uint8 bgColorFrame = 150;
  Uint8 bgColorFrameShade = 172;
  Uint8 bgColorBottomFrame = 106;
  Uint8 bgColorBottomFrameShade = 84;
  
  SDL_Color color = { 128, 128, 128 };
  SDL_Color color_temp = { 128, 128, 128 };

  SDL_Rect buttonrect = rect;

 
  drawSolidRect(rect, SDL_Color{ bgColorFrame, bgColorFrame, bgColorFrame });
  drawSolidRect(SDL_Rect{ rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4 }, SDL_Color{ bgColorFrameShade, bgColorFrameShade, bgColorFrameShade });
  drawSolidRect(SDL_Rect{ rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8 }, SDL_Color{ bgColorFrame, bgColorFrame, bgColorFrame });
  drawSolidRect(SDL_Rect{ rect.x + 6, rect.y + 6, rect.w - 12, rect.h - 12 }, SDL_Color{ bgColor, bgColor, bgColor });
  
}