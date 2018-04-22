#include "sprite.hxx"


Sprite::Sprite(std::string filename, int x, int y, SDL_Renderer *renderer, SDL_Window *window)
{

  _texture = nullptr;
  _renderer = nullptr;
  _window = nullptr;

  _texture = loadTexture(filename, renderer, true);

  _renderer = renderer;
  _window = window;

  setPosX(x);
  setPosY(y);

  setDestX(x);
  setDestY(y);

  setMoving(false);
}

Sprite::~Sprite()
{
  //Destructor necessary?
}

void Sprite::render(int cameraoffset_x, int cameraoffset_y, float zoom, int height)
{
  float pixel_x = 0;
  float pixel_y = 0;

  pixel_y = getPixelY(cameraoffset_x, cameraoffset_y, zoom) - height;
  pixel_x = getPixelX(cameraoffset_x, cameraoffset_y, zoom);

  //Render only whats visible
  const int offscreen_tolerance = 3 * TILE_SIZE*zoom;
  int screen_width;
  int screen_height;
  SDL_GetWindowSize(_window, &screen_width, &screen_height);

  if ((pixel_x >= 0 - offscreen_tolerance) ||
    (pixel_x + TILE_SIZE * zoom <= screen_width + offscreen_tolerance) ||
    (pixel_y >= 0 - offscreen_tolerance) ||
    (pixel_y + TILE_SIZE * zoom <= screen_height + offscreen_tolerance))
  {
    renderTexture(_texture, _renderer, pixel_x, pixel_y, TILE_SIZE*zoom, TILE_SIZE*zoom);
  }
}

float Sprite::getPixelX(int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pixel_x = 0;
  pixel_x = (TILE_SIZE*zoom * getPosX() * 0.5) + (TILE_SIZE*zoom * getPosY() * 0.5) - cameraoffset_x;

  return pixel_x;
}

float Sprite::getPixelY(int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pixel_y = 0;
  pixel_y = ((TILE_SIZE*zoom * getPosX() * 0.25) - (TILE_SIZE*zoom * getPosY() * 0.25)) - cameraoffset_y;

  return pixel_y;
}

float Sprite::getZOrder()
{
  return getPixelY(0, 0);
}
