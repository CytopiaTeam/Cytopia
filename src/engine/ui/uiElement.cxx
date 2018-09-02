#include "uiElement.hxx"

UiElement::UiElement(const SDL_Rect& uiElementRect) : _uiElementRect(uiElementRect) {}

void UiElement::draw()
{
  if (_texture)
  {
    renderTexture();
  }
}

void UiElement::setSpriteID(int uiSpriteID)
{
  _uiSpriteID = uiSpriteID;
  _texture = TextureManager::Instance().getUITexture(_uiSpriteID, TextureManager::buttonState::DEFAULT);
  SDL_QueryTexture(_texture, nullptr, nullptr, &_uiElementRect.w, &_uiElementRect.h);
}

void UiElement::changeButtonState(int state)
{
  if (_uiSpriteID != -1 && _buttonState != state)
  {
    changeTexture(TextureManager::Instance().getUITexture(_uiSpriteID, state));
  }
  _buttonState = state;
}

void UiElement::renderTexture()
{
  if (_texture)
  {
    SDL_Rect destRect;
    if (_textRect.w != 0 && !_textBlittedToTexture)
    {
      destRect = _textRect;
    }
    else if (_uiElementRect.w != 0)
    {
      destRect = _uiElementRect;
    }
    SDL_RenderCopy(_renderer, _texture, nullptr, &destRect);
  }
}

bool UiElement::isClicked(int x, int y)
{
  return x > _uiElementRect.x && x < _uiElementRect.x + _uiElementRect.w && y > _uiElementRect.y &&
         y < _uiElementRect.y + _uiElementRect.h;
}

void UiElement::setText(const std::string &text)
{
  _text = text;
  drawText(_text, SDL_Color{255, 255, 255});
}

void UiElement::drawText(const std::string &text, const SDL_Color &textColor)
{
  _font = TTF_OpenFont("resources/fonts/arcadeclassics.ttf", 20);

  if (!_font)
  {
    LOG(LOG_ERROR) << "Failed to load font!\n" << TTF_GetError();
  }

  // destroy texture first to prevent memleaks
  if (_texture)
    SDL_DestroyTexture(_texture);

  SDL_Surface *textSurface = TTF_RenderText_Solid(_font, text.c_str(), textColor);
  if (textSurface)
  {
    // If there's already an existing surface (like a button) blit the text to it.
    if (_surface)
    {
      // Center the text in the surface
      _textRect.x = (_surface->w / 2) - (_textRect.w / 2);
      _textRect.y = (_surface->h / 2) - (_textRect.h / 2);
      _textRect.w = _surface->w;
      _textRect.h = _surface->h;
      SDL_BlitSurface(textSurface, nullptr, _surface, &_textRect);
      _texture = SDL_CreateTextureFromSurface(_renderer, _surface);
      _textBlittedToTexture = true;

      return;
    }
    // else just create a new text texture
    else
    {
      _texture = SDL_CreateTextureFromSurface(_renderer, textSurface);

      // no surface exists but some shape has been drawn for that ui element
      SDL_QueryTexture(_texture, nullptr, nullptr, &_textRect.w, &_textRect.h);

      if (_uiElementRect.w != 0)
      {
        int textWidth, textHeight;
        SDL_QueryTexture(_texture, nullptr, nullptr, &textWidth, &textHeight);
        _textRect.x = _uiElementRect.x + (_uiElementRect.w / 2) - (_textRect.w / 2);
        _textRect.y = _uiElementRect.y + (_uiElementRect.h / 2) - (_textRect.h / 2);
      }
      // only the text field exists
      else
      {
        _textRect.x = _uiElementRect.x;
        _textRect.y = _uiElementRect.y;
        _uiElementRect = _textRect;
      }
    }

    if (!_texture)
    {
      LOG(LOG_ERROR) << "Failed to create texture from text surface!\n" << SDL_GetError();
    }
    //Delete no longer needed surface
    SDL_FreeSurface(textSurface);
  }
  else
  {
    LOG(LOG_ERROR) << "Failed to create text surface!\n" << TTF_GetError();
  }

  TTF_CloseFont(_font);
}

void UiElement::drawTextFrame()
{
  if (_textRect.w != 0 && _textRect.h != 0)
  {
    SDL_Rect textFrameRect = _textRect;
    textFrameRect.x = textFrameRect.x - 10;
    textFrameRect.w = textFrameRect.w + 20;
    drawSolidRect(textFrameRect, SDL_Color{150, 150, 150});
    drawSolidRect({textFrameRect.x + 1, textFrameRect.y + 1, textFrameRect.w - 2, textFrameRect.h - 2}, SDL_Color{128, 128, 128});
  }
}

void UiElement::drawSolidRect(SDL_Rect rect, const SDL_Color &color) const
{
  SDL_Renderer *renderer = Resources::getRenderer();
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &rect);
}

void UiElement::drawLine(int x1, int y1, int x2, int y2, const SDL_Color &color) const
{
  SDL_Renderer *renderer = Resources::getRenderer();
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void UiElement::drawButtonFrame(SDL_Rect rect, bool isHighlightable)
{
  Uint8 bgColor, bgColorFrame, bgColorFrameShade, bgColorBottomFrame, bgColorBottomFrameShade;

  if ((getButtonState() == TextureManager::buttonState::CLICKED || getButtonState() == TextureManager::buttonState::TOGGLED) &&
      isHighlightable)
  {
    bgColor = 128;
    bgColorFrame = 106;
    bgColorFrameShade = 84;
    bgColorBottomFrame = 150;
    bgColorBottomFrameShade = 172;
  }
  else if ((getButtonState() == TextureManager::buttonState::HOVERING) && isHighlightable)
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
}