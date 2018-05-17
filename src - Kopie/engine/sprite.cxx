#include "sprite.hxx"

Sprite::Sprite(int tileID, Point isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  

  _isoCoordinates = isoCoordinates;
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);

  _texture = TextureManager::Instance().getTexture(tileID);
}


Sprite::~Sprite()
{
  SDL_DestroyTexture(_texture);
}

void Sprite::render()
{
  float _zoomLevel = Resources::getZoomLevel();
  int TILE_SIZE = Resources::getTileSize();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);

  int offscreenTolerance = 3 * TILE_SIZE * _zoomLevel;
  int screen_width, screen_height;
  SDL_GetWindowSize(_window, &screen_width, &screen_height);

  //Render only whats visible
  if (( _screenCoordinates.getX() >= 0 - offscreenTolerance )
  ||  ( _screenCoordinates.getX() + TILE_SIZE * _zoomLevel <= screen_width + offscreenTolerance )
  ||  ( _screenCoordinates.getY() >= 0 - offscreenTolerance )
  ||  ( _screenCoordinates.getY() + TILE_SIZE * _zoomLevel <= screen_height + offscreenTolerance ))
  {
    renderTexture(TILE_SIZE * _zoomLevel, TILE_SIZE * _zoomLevel);
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
  bool drawBorder = false;

  SDL_Rect destRect;
  destRect.x = _screenCoordinates.getX();
  destRect.y = _screenCoordinates.getY();
  destRect.w = w;
  destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &destRect);

  if (drawBorder == true)
  {
    SDL_RenderDrawRect(_renderer, &destRect);
  }
}

void Sprite::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  renderTexture(width, height);
}

SDL_Rect Sprite::textureInformation()
{
  int width, height;
  SDL_Rect spriteInformation;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  spriteInformation.w = width;
  spriteInformation.h = height;
  spriteInformation.x = _screenCoordinates.getX();
  spriteInformation.y = _screenCoordinates.getY();
  
  return spriteInformation;
}
