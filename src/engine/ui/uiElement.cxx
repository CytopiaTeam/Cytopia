#include "uiElement.hxx"

UiElement::UiElement(int x, int y, int uiSpriteID) : _screenCoordinates(Point(x,y))
{
  _texture = TextureManager::Instance().getUITexture(uiSpriteID);
}

UiElement::UiElement(int x, int y, std::string text) : _screenCoordinates(Point(x, y))
{
  drawText(text, _color);
}

UiElement::UiElement(int x, int y, int w, int h) : _screenCoordinates(Point(x, y)), _width(w), _height(h)
{

}


void UiElement::render()
{
  if (isVisible())
  {
    renderTexture();
  }
}

void UiElement::changeTexture(int tileID)
{
  _texture = TextureManager::Instance().getUITexture(tileID);
}

void UiElement::renderTexture(int w, int h)
{
  _destRect.x = _screenCoordinates.getX();
  _destRect.y = _screenCoordinates.getY();
  _destRect.w = w;
  _destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void UiElement::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  if (_texture)
  {
    renderTexture(width, height);
  }
}

bool UiElement::isClicked(int x, int y)
{
  if (x > _destRect.x && x < _destRect.x + _destRect.w
  &&  y > _destRect.y && y < _destRect.y + _destRect.h)
  {
    return true;
  }
  return false;
}


void UiElement::drawText(const std::string &textureText, const SDL_Color& textColor)
{
  _font = TTF_OpenFont("resources/fonts/arcadeclassics.ttf", 20);

  if ( ! _font )
  {
    LOG(LOG_ERROR) << "Failed to load font!\n" << TTF_GetError();
  }

  SDL_Surface* textSurface = TTF_RenderText_Solid(_font, textureText.c_str(), textColor);
  if ( textSurface )
  {
    _width = textSurface->w;
    _height = textSurface->h;

    // If there's already an existing surface (like a button) blit the text to it.
    if ( _surface )
    {
      // Center the text in the surface
      SDL_Rect textLocation = { 0, 0, 0, 0 };
      textLocation.x = (_surface->w / 2) - (_width / 2);
      textLocation.y = (_surface->h / 2) - (_height / 2);
      SDL_BlitSurface(textSurface, NULL, _surface, &textLocation);
      _texture = SDL_CreateTextureFromSurface(_renderer, _surface);

      return;
    }
    // else just create a new text texture
    else
    {
      _texture = SDL_CreateTextureFromSurface(_renderer, textSurface);
    }


    if ( ! _texture )
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

void UiElement::drawSolidRect(SDL_Rect& rect, const SDL_Color& color)
{
  _destRect = rect;
  _surface = SDL_CreateRGBSurface(0, _destRect.w, _destRect.h, 32, 0, 0, 0, 0);;

  // Use NULL to fill whole surface with color
  SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, color.r, color.g, color.b));
  _texture = SDL_CreateTextureFromSurface(_renderer, _surface);
}

bool UiElement::isVisible()
{
  return _visible;
}