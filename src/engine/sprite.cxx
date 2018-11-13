#include "sprite.hxx"

#include "textureManager.hxx"
#include "basics/resources.hxx"

Sprite::Sprite(Point isoCoordinates) : _isoCoordinates(isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _zoomLevel = Resources::getZoomLevel();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);
}

void Sprite::renderNew()
{

  // don't use rendercopyex
  if (_spriteCount > 1)
  {
    SDL_RenderCopy(_renderer, _texture, &_clipRect, &_destRect);
  }
  else
  {
    SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
  }
}

//
//void Sprite::render(SDL_rect clipRect)
//{
//
//  SDL_Rect clipRect{ 0,0, 32,32 };
//  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
//  SDL_RenderCopyEx(_renderer, _texture, nullptr, &_destRect, 0, nullptr, SDL_FLIP_NONE);
//}

void Sprite::refresh()
{
  if (_zoomLevel != Resources::getZoomLevel())
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
  _destRect.y = _screenCoordinates.y;
}

void Sprite::setTextureNew(SDL_Texture *texture)
{
  SDL_QueryTexture(texture, nullptr, nullptr, &_destRect.w, &_destRect.h);

  _texture = texture;
  _tileSize = _destRect.w;

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
};
