#include "combobox.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), rect(uiElementRect)
{ 
  menuRect = rect;
  menuRect.y = rect.y + rect.h;
  menuRect.h = 100;

  _textField = std::make_shared<TextField>(TextField(menuRect));
  _textField->addText("test");
  _textField->addText("awesome element");
  _textField->addText("one more element");
}

void ComboBox::draw()
{
  Uint8 bgColor, bgColorFrame, bgColorFrameShade, bgColorBottomFrame, bgColorBottomFrameShade;

  if (getButtonState() == TextureManager::CLICKED || getButtonState() == TextureManager::TOGGLED)
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

  // draw the button frame
  if (rect.w != 0 && rect.h != 0)
  {
    drawButtonFrame(rect);
  }

  // arrow

  SDL_Rect arrowRect;
  SDL_Color color = {bgColorFrameShade, bgColorFrameShade, bgColorFrameShade};

  arrowRect.w = 18;
  arrowRect.h = 2;
  arrowRect.x = rect.x + (rect.w - arrowRect.w) - 9;
  arrowRect.y = rect.y + (rect.h / 2) - (arrowRect.w / 2) / arrowRect.h - 1;

  for (; arrowRect.w > 1; arrowRect.w -= 2)
  {
    drawSolidRect(arrowRect, color);
    arrowRect.x++;
    arrowRect.y++;
  }
  drawSolidRect(arrowRect, color);

  // drowpdowb box

  bool isMenuOpened = true;

  
  if (isMenuOpened)
  {

    // top frame
    drawSolidRect(menuRect, SDL_Color{bgColorFrameShade, bgColorFrameShade, bgColorFrameShade});
    drawSolidRect(SDL_Rect{menuRect.x + 2, menuRect.y + 2, menuRect.w - 4, menuRect.h - 4},
                  SDL_Color{bgColorFrame, bgColorFrame, bgColorFrame});
    // background
    drawSolidRect(SDL_Rect{menuRect.x + 4, menuRect.y + 4, menuRect.w - 8, menuRect.h - 8}, SDL_Color{bgColor, bgColor, bgColor});
    // bottom frame
    drawSolidRect(SDL_Rect{menuRect.x + 4, (menuRect.y + menuRect.h) - 4, menuRect.w - 4, 4},
                  SDL_Color{bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame});
    drawSolidRect(SDL_Rect{(menuRect.x + menuRect.w) - 4, menuRect.y + 4, 4, menuRect.h - 4},
                  SDL_Color{bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame});
    // bottom frame shade
    drawSolidRect(SDL_Rect{menuRect.x + 2, (menuRect.y + menuRect.h) - 2, menuRect.w - 2, 2},
                  SDL_Color{bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade});
    drawSolidRect(SDL_Rect{(menuRect.x + menuRect.w) - 2, menuRect.y + 2, 2, menuRect.h - 2},
                  SDL_Color{bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade});

    drawText("test", {255, 255, 255});
  }

  _textField->draw();

  //render the buttons texture if available
  renderTexture();
}
