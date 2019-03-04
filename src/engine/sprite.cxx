#include "sprite.hxx"

#include "resourcesManager.hxx"
#include "windowManager.hxx"
#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/log.hxx"

Sprite::Sprite(Point isoCoordinates) : _isoCoordinates(isoCoordinates)
{
  _screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates);
}

void Sprite::render()
{
  if (_highlightSprite == true)
  {
    SDL_SetTextureColorMod(_texture, 150, 150, 150);
  }

  if (_clipRect.w != 0)
  {
    SDL_RenderCopy(WindowManager::instance().getRenderer(), _texture, &_clipRect, &_destRect);
  }
  else
  {
    SDL_RenderCopy(WindowManager::instance().getRenderer(), _texture, nullptr, &_destRect);
  }

  if (_highlightSprite == true)
  {
    SDL_SetTextureColorMod(_texture, 255, 255, 255);
  }
}

void Sprite::refresh()
{
  if (_currentZoomLevel != Camera::zoomLevel || _needsRefresh)
  {
    _currentZoomLevel = Camera::zoomLevel;
    if (_clipRect.w != 0)
    {
      _destRect.w = static_cast<int>(_clipRect.w * _currentZoomLevel);
      _destRect.h = static_cast<int>(_clipRect.h * _currentZoomLevel);
    }
    else
    {
      SDL_QueryTexture(_texture, nullptr, nullptr, &_destRect.w, &_destRect.h);
      _destRect.w = static_cast<int>(_destRect.w * _currentZoomLevel);
      _destRect.h = static_cast<int>(_destRect.h * _currentZoomLevel);
    }
  }

  _screenCoordinates = convertIsoToScreenCoordinates(_isoCoordinates);
  // render the sprite in the middle of its bounding box so bigger than 1x1 sprites will render correctly
  _destRect.x = _screenCoordinates.x - (_destRect.w / 2);
  // change y coordinates with sprites height taken into account to render the sprite at its base and not at its top.
  _destRect.y = _screenCoordinates.y - _destRect.h;
  _needsRefresh = false;
}

void Sprite::setTexture(SDL_Texture *texture)
{
  _texture = texture;
  _needsRefresh = true;
  refresh();
}
