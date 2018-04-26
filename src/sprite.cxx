#include "sprite.hxx"


Sprite::Sprite(std::string filename, int x, int y, SDL_Renderer *renderer, SDL_Window *window)
{

  _texture = nullptr;
  _renderer = nullptr;
  _window = nullptr;

  _texture = loadTexture(filename, renderer, true);

  _renderer = renderer;
  _window = window;

  setTileIsoCoordinates(x, y);
}

Sprite::~Sprite()
{
  //Destructor necessary?
}

void Sprite::render(Point cameraOffset, float zoom, int height)
{

  Point tileScreenCoords = getTileScreenCoordinates(cameraOffset, zoom);
  tileScreenCoords.y -= height;

  //Render only whats visible
  const int offscreen_tolerance = 3 * TILE_SIZE*zoom;
  int screen_width;
  int screen_height;
  SDL_GetWindowSize(_window, &screen_width, &screen_height);

  if ((tileScreenCoords.x >= 0 - offscreen_tolerance) ||
    (tileScreenCoords.x + TILE_SIZE * zoom <= screen_width + offscreen_tolerance) ||
    (tileScreenCoords.y >= 0 - offscreen_tolerance) ||
    (tileScreenCoords.y + TILE_SIZE * zoom <= screen_height + offscreen_tolerance))
  {
    renderTexture(_texture, _renderer, tileScreenCoords, TILE_SIZE*zoom, TILE_SIZE*zoom);
  }
}

Point Sprite::getTileScreenCoordinates(Point cameraOffset, float zoom)
{
  Point Coords;
  Coords.x = (TILE_SIZE*zoom * getTileIsoCoordinates().x * 0.5) + (TILE_SIZE*zoom * getTileIsoCoordinates().y * 0.5) - cameraOffset.x;
  Coords.y = ((TILE_SIZE*zoom * getTileIsoCoordinates().x * 0.25) - (TILE_SIZE*zoom * getTileIsoCoordinates().y * 0.25)) - cameraOffset.y;

  return Coords;
}

int Sprite::getZOrder()
{
   // TODO: Implement...
  return 0;
}
