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
  _destRect.h = static_cast<int>(16 * _zoomLevel);

  if (_isoCoordinates.x == 30 && _isoCoordinates.y == 30)
  {
    _texture = TextureManager::Instance().getTileTextureNew("paved_road");
  }
}

void Sprite::renderNew()
{

  if (_spriteCount > 1)
    SDL_RenderCopyEx(_renderer, _texture, &_clipRect, &_destRect, 0, nullptr, SDL_FLIP_NONE);
  else
    SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void Sprite::render()
{
  if (_isoCoordinates.x == 30 && _isoCoordinates.y == 30)
  {

    SDL_Rect clipRect{32, 0, 32, 16};
    //_destRect.h = 32;

    //SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
    SDL_RenderCopyEx(_renderer, _texture, &clipRect, &_destRect, 0, nullptr, SDL_FLIP_NONE);
  }
  else
    SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
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
    _destRect.w = static_cast<int>(_tileSize * _zoomLevel);
    _destRect.h = static_cast<int>(16 * _zoomLevel);
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
  _destRect.h = static_cast<int>(16 * _zoomLevel);

  if (_isoCoordinates.x == 30 && _isoCoordinates.y == 30)
  {
    _texture = TextureManager::Instance().getTileTextureNew("paved_road");
  }
};

void Sprite::setTextureNew(SDL_Texture *texture)
{
  SDL_QueryTexture(texture, nullptr, nullptr, &_destRect.w, &_destRect.h);

  _texture = texture;
  _tileSize = _destRect.w;
  _destRect.w = static_cast<int>(_tileSize * _zoomLevel);
  _destRect.h = static_cast<int>(16 * _zoomLevel);

  if (_isoCoordinates.x == 30 && _isoCoordinates.y == 30)
  {
    _texture = TextureManager::Instance().getTileTextureNew("paved_road");
  }
};
