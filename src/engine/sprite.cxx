#include "sprite.hxx"

Sprite::Sprite(int tileID, Point isoCoordinates) : _tileID(tileID), _isoCoordinates(isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _isoCoordinates = isoCoordinates; 
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  _texture = TextureManager::Instance().getTileTexture(tileID);
}

void Sprite::setTileIsoCoordinates(Point isoCoords) 
{ 
  _isoCoordinates = isoCoords; 
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
}


void Sprite::render()
{
  int tileSize = static_cast<int>(Resources::getTileSize() * Resources::getZoomLevel());

  int offscreenTolerance = static_cast<int>(3 * tileSize);
  int screen_width = Settings::Instance().settings.screenWidth;
  int screen_height = Settings::Instance().settings.screenHeight;

  //Render only whats visible
  if ((_screenCoordinates.x >= 0 - offscreenTolerance) ||
      (_screenCoordinates.x + tileSize <= screen_width + offscreenTolerance) ||
      (_screenCoordinates.y >= 0 - offscreenTolerance) || (_screenCoordinates.y + tileSize <= screen_height + offscreenTolerance))
  {
    renderTexture(tileSize, tileSize);
  }
}

void Sprite::renderTexture(int w, int h)
{
  _destRect.x = _screenCoordinates.x;
  _destRect.y = _screenCoordinates.y;
  _destRect.w = w;
  _destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void Sprite::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, nullptr, nullptr, &width, &height);
  renderTexture(width, height);
}
