#include "sprite.hxx"

#include "textureManager.hxx"
#include "basics/resources.hxx"
#include "basics/log.hxx"

Sprite::Sprite(Point isoCoordinates) : _isoCoordinates(isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);
}

void Sprite::render()
{

  if (_spriteCount > 1)
  {
    SDL_RenderCopy(_renderer, _texture, &_clipRect, &_destRect);
  }
  else
  {
    SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
  }
}

void Sprite::refresh()
{
  if (_zoomLevel != Resources::getZoomLevel() || _needsRefresh)
  {
    _zoomLevel = Resources::getZoomLevel();
    if (_spriteCount > 1)
    {
      _destRect.w = static_cast<int>(_clipRect.w * _zoomLevel);
      _destRect.h = static_cast<int>(_clipRect.h * _zoomLevel);
    }
    else
    {
      SDL_QueryTexture(_texture, nullptr, nullptr, &_destRect.w, &_destRect.h);
      _destRect.w = static_cast<int>(_destRect.w * _zoomLevel);
      _destRect.h = static_cast<int>(_destRect.h * _zoomLevel);
    }
  }

  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);
  _destRect.x = _screenCoordinates.x;
  _destRect.y = _screenCoordinates.y - _destRect.h;
  _needsRefresh = false;
}

void Sprite::setTexture(SDL_Texture *texture)
{
  _texture = texture;
  _needsRefresh = true;
  refresh();
};
