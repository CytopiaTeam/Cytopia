#include "uiElement.hxx"

UiElement::UiElement(int x, int y, int uiSpriteID, int groupID, int actionID, int parentOfGroup)
    : _uiElementRect({ x, y, 0, 0 }), _groupID(groupID), _actionID(actionID), _parentOf(parentOfGroup),
      _uiElementType("ImageButton")
{
  _texture = TextureManager::Instance().getUITexture(uiSpriteID);
  SDL_QueryTexture(_texture, NULL, NULL, &_uiElementRect.w, &_uiElementRect.h);
}

UiElement::UiElement(int x, int y, const std::string &text, int groupID, int actionID, int parentOfGroup)
    : _uiElementRect({ x, y, 0, 0 }), _groupID(groupID), _actionID(actionID), _parentOf(parentOfGroup)
{
  drawText(text, _color);
}

UiElement::UiElement(int x, int y, int w, int h, int groupID, int actionID, int parentOfGroup)
    : _uiElementRect({x, y, w, h}), _groupID(groupID), _actionID(actionID), _parentOf(parentOfGroup)
{
}

void UiElement::draw() 
{ 
  if (_texture)
  {
    renderTexture(); 
  }
}

void UiElement::changeTexture(int tileID) { _texture = TextureManager::Instance().getUITexture(tileID); }

void UiElement::renderTexture()
{
  if (_texture)
  {
    SDL_Rect destRect;
    if (_textRect.w != 0 && _textBlittedToTexture == false)
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
  if (x > _uiElementRect.x && x < _uiElementRect.x + _uiElementRect.w && y > _uiElementRect.y && y < _uiElementRect.y + _uiElementRect.h)
  {
    return true;
  }
  return false;
}

void UiElement::drawText(const std::string &textureText, const SDL_Color &textColor)
{
  _font = TTF_OpenFont("resources/fonts/arcadeclassics.ttf", 20);

  if (!_font)
  {
    LOG(LOG_ERROR) << "Failed to load font!\n" << TTF_GetError();
  }

  SDL_Surface *textSurface = TTF_RenderText_Solid(_font, textureText.c_str(), textColor);
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
      SDL_BlitSurface(textSurface, NULL, _surface, &_textRect);
      _texture = SDL_CreateTextureFromSurface(_renderer, _surface);
      _textBlittedToTexture = true;

      return;
    }
    // else just create a new text texture
    else
    {
    _texture = SDL_CreateTextureFromSurface(_renderer, textSurface);
 
    // no surface exists but some shape has been drawn for that ui element
      SDL_QueryTexture(_texture, NULL, NULL, &_textRect.w, &_textRect.h);
      
      if (_uiElementRect.w != 0)
      {
        int textWidth, textHeight;
        SDL_QueryTexture(_texture, NULL, NULL, &textWidth, &textHeight);
        _textRect.x = _uiElementRect.x + (_uiElementRect.w / 2) - (_textRect.w / 2);
        _textRect.y = _uiElementRect.y + (_uiElementRect.h / 2) - (_textRect.h / 2);
      }
      // only the text field exists
      else
      {
        _textRect.x = _uiElementRect.x;
        _textRect.y = _uiElementRect.y;
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

void UiElement::drawSolidRect(SDL_Rect rect, const SDL_Color &color)
{
  SDL_Renderer* renderer = Resources::getRenderer();
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &rect);
}

void UiElement::drawLine(int x1, int y1, int x2, int y2, const SDL_Color &color)
{
  SDL_Renderer* renderer = Resources::getRenderer();
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}