#include "button.hxx"

Button::Button(int x, int y, int w, int h, const std::string &text, int groupID, int actionID, int parentOfGroup)
    : UiElement(x, y, w, h, groupID, actionID, parentOfGroup)
{
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  drawText(text, SDL_Color{255, 255, 255});


}

void Button::draw()
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
    
  // top frame
  drawSolidRect(rect, SDL_Color{ bgColorFrameShade, bgColorFrameShade, bgColorFrameShade });
  drawSolidRect(SDL_Rect{ rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4 }, SDL_Color{ bgColorFrame, bgColorFrame, bgColorFrame });
  // background
  drawSolidRect(SDL_Rect{ rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8 }, SDL_Color{ bgColor, bgColor, bgColor });
  // bottom frame
  drawSolidRect(SDL_Rect{ rect.x + 4, (rect.y + rect.h) - 4, rect.w - 4, 4 }, SDL_Color{ bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame });
  drawSolidRect(SDL_Rect{ (rect.x + rect.w) - 4, rect.y+4, 4, rect.h - 4 }, SDL_Color{ bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame });
  // bottom frame shade
  drawSolidRect(SDL_Rect{ rect.x + 2, (rect.y + rect.h) - 2, rect.w - 2, 2 }, SDL_Color{ bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade });
  drawSolidRect(SDL_Rect{ (rect.x + rect.w) - 2, rect.y+2, 2, rect.h - 2 }, SDL_Color{ bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade });
  renderTexture();
  
}