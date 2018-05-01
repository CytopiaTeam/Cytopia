#include "sprite.hxx"


Sprite::Sprite(std::string filename, Point isoCoords, SDL_Renderer *renderer, SDL_Window *window)
{

  _texture = nullptr;
  _renderer = nullptr;
  _window = nullptr;

  _texture = loadTexture(filename, renderer, true);

  _renderer = renderer;
  _window = window;

  _isoCoords = isoCoords;
}

Sprite::~Sprite()
{

}

void Sprite::render(Point cameraOffset, float zoom)
{
  Point tileScreenCoords = getTileScreenCoordinates(cameraOffset, zoom);

  if ( _isoCoords.getHeight() > 0 )
  {
    _heightOffset = _heightOffset * _isoCoords.getHeight();
    tileScreenCoords.setY(tileScreenCoords.getY() - (TILE_SIZE - _heightOffset)*zoom); 
  }

  //Render only whats visible
  const int offscreen_tolerance = 3 * TILE_SIZE*zoom;
  int screen_width;
  int screen_height;
  SDL_GetWindowSize(_window, &screen_width, &screen_height);

  if (( tileScreenCoords.getX() >= 0 - offscreen_tolerance ) ||
      ( tileScreenCoords.getX() + TILE_SIZE * zoom <= screen_width + offscreen_tolerance ) ||
      ( tileScreenCoords.getY() >= 0 - offscreen_tolerance ) ||
      ( tileScreenCoords.getY() + TILE_SIZE * zoom <= screen_height + offscreen_tolerance ))
  {
    renderTexture(_texture, _renderer, tileScreenCoords, TILE_SIZE*zoom, TILE_SIZE*zoom);
  }
}

Point Sprite::getTileScreenCoordinates(Point cameraOffset, float zoom)
{
  Point tileScreenCoords;
  int x, y;

  x = (TILE_SIZE*zoom * _isoCoords.getX() * 0.5) + (TILE_SIZE*zoom * _isoCoords.getY() * 0.5) - cameraOffset.getX();
  y = ((TILE_SIZE*zoom * _isoCoords.getX() * 0.25) - (TILE_SIZE*zoom * _isoCoords.getY() * 0.25)) - cameraOffset.getY();

  tileScreenCoords.setCoords(x, y);
  return tileScreenCoords;
}

int Sprite::getZOrder()
{
   // TODO: Implement...
  return 0;
}

void Sprite::setTileIsoCoordinates(Point isoCoords)
{
  _isoCoords = isoCoords;
}

Point Sprite::getTileIsoCoordinates()
{
  return _isoCoords;
}

void Sprite::setHeight(int height)
{
  _isoCoords.setHeight(height);
}
