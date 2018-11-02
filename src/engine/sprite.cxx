#include "sprite.hxx"

#include "textureManager.hxx"
#include "basics/resources.hxx"

Sprite::Sprite(SDL_Texture *texture, Point isoCoordinates) : _texture(texture), _isoCoordinates(isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);
  SDL_QueryTexture(_texture, nullptr, nullptr, &_destRect.w, &_destRect.h);
  _zoomLevel = Resources::getZoomLevel();
  _tileSize = static_cast<int>(Resources::getTileSize() * Resources::getZoomLevel());
  offscreenTolerance = 3 * _tileSize;
}

void Sprite::render() { SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect); }

void Sprite::refresh()
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

void Sprite::setTexture(SDL_Texture *texture) 
{
  _texture = texture;
  SDL_QueryTexture(_texture, nullptr, nullptr, &_destRect.w, &_destRect.h);
};
