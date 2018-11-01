#include "sprite.hxx"

#include "textureManager.hxx"
#include "basics/resources.hxx"

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
}

void Sprite::render() { SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect); }

void Sprite::updateCoordinates()
{
  if (_zoomLevel != Resources::getZoomLevel())
  {
    _zoomLevel = Resources::getZoomLevel();
    _tileSize = static_cast<int>(Resources::getTileSize() * Resources::getZoomLevel());
    _destRect.w = _tileSize;
    _destRect.h = _tileSize;
    offscreenTolerance = 3 * _tileSize;
  }

  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  _destRect.x = _screenCoordinates.x;
  _destRect.y = _screenCoordinates.y;
}

void Sprite::changeTexture(int tileID) { _texture = TextureManager::Instance().getTileTexture(tileID); }