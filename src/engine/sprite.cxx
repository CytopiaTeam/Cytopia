#include "sprite.hxx"

Sprite::Sprite(int tileID, Point isoCoordinates) : _tileID(tileID), _isoCoordinates(isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);
  _texture = TextureManager::Instance().getTileTexture(tileID);
  SDL_QueryTexture(_texture, nullptr, nullptr, &_destRect.w, &_destRect.h);
  _zoomLevel = Resources::getZoomLevel();
  _tileSize = static_cast<int>(Resources::getTileSize() * Resources::getZoomLevel());
  offscreenTolerance = 3 * _tileSize;

  // TODO: This won't work once resolution is changed.
  screen_width = Settings::Instance().settings.screenWidth;
  screen_height = Settings::Instance().settings.screenHeight;
}

void Sprite::render()
{
  if (_zoomLevel != Resources::getZoomLevel())
  {
    _zoomLevel = Resources::getZoomLevel();
    _tileSize = static_cast<int>(Resources::getTileSize() * Resources::getZoomLevel());
    _destRect.w = _tileSize;
    _destRect.h = _tileSize;
    offscreenTolerance = 3 * _tileSize;
  }

  //Render only whats visible - this doesn't work
  if ((_screenCoordinates.x >= 0 - offscreenTolerance) ||
      (_screenCoordinates.x + _tileSize <= screen_width + offscreenTolerance) ||
      (_screenCoordinates.y >= 0 - offscreenTolerance) ||
      (_screenCoordinates.y + _tileSize <= screen_height + offscreenTolerance))
  {
    SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
  }
}

void Sprite::changeTexture(int tileID)
{
  _texture = TextureManager::Instance().getTileTexture(tileID);
}

void Sprite::setTileIsoCoordinates(Point isoCoords) { _isoCoordinates = isoCoords; }

void Sprite::updateCoordinates()
{
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  _destRect.x = _screenCoordinates.x;
  _destRect.y = _screenCoordinates.y;
}