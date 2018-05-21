#include "sprite.hxx"

Sprite::Sprite(int tileID, Point isoCoordinates) : _tileID(tileID), _isoCoordinates(isoCoordinates) 
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  

  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);

  _texture = TextureManager::Instance().getTexture(tileID);
}


Sprite::~Sprite()
{

}

void Sprite::render()
{
  int tileSize = static_cast<int>(Resources::getTileSize() * Resources::getZoomLevel());
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);

  int offscreenTolerance = static_cast<int>(3 * tileSize);
  int screen_width = Resources::settings.screenWidth; 
  int screen_height = Resources::settings.screenHeight;

  //Render only whats visible
  if (( _screenCoordinates.getX() >= 0 - offscreenTolerance )
  ||  ( _screenCoordinates.getX() + tileSize <= screen_width + offscreenTolerance )
  ||  ( _screenCoordinates.getY() >= 0 - offscreenTolerance )
  ||  ( _screenCoordinates.getY() + tileSize <= screen_height + offscreenTolerance ))
  {
    renderTexture(tileSize, tileSize);
  }
}

void Sprite::setTileIsoCoordinates(Point isoCoords)
{
  _isoCoordinates = isoCoords;
}

void Sprite::changeTexture(int tileID)
{
  _texture = TextureManager::Instance().getTexture(tileID);
}

void Sprite::renderTexture(int w, int h)
{
  _destRect.x = _screenCoordinates.getX();
  _destRect.y = _screenCoordinates.getY();
  _destRect.w = w;
  _destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void Sprite::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  renderTexture(width, height);
}

SDL_Rect Sprite::textureInformation()
{
  return _destRect;
}
