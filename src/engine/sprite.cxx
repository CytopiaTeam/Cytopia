#include "sprite.hxx"

#include "textureManager.hxx"
#include "basics/resources.hxx"

Sprite::Sprite(SDL_Texture *texture, Point isoCoordinates) : _texture(texture), _isoCoordinates(isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _zoomLevel = Resources::getZoomLevel();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);

  SDL_QueryTexture(_texture, nullptr, nullptr, &_destRect.w, &_destRect.h);

  _tileSize = _destRect.w; // tile is always a square
  _destRect.w = static_cast<int>(_tileSize * _zoomLevel);
  _destRect.h = static_cast<int>(_tileSize * _zoomLevel);
}

void Sprite::render() { SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect); }

void Sprite::refresh()
{
  if (_zoomLevel != Resources::getZoomLevel())
  {
    _zoomLevel = Resources::getZoomLevel();
    _destRect.w = static_cast<int>(_tileSize * _zoomLevel);
    _destRect.h = static_cast<int>(_tileSize * _zoomLevel);
  }

  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  _destRect.x = _screenCoordinates.x;
  _destRect.y = _screenCoordinates.y;
}

void Sprite::setTexture(SDL_Texture *texture)
{
  SDL_QueryTexture(texture, nullptr, nullptr, &_destRect.w, &_destRect.h);

  _texture = texture;
  _tileSize = _destRect.w;
  _destRect.w = static_cast<int>(_tileSize * _zoomLevel);
  _destRect.h = static_cast<int>(_tileSize * _zoomLevel);
};
