#include "sprite.hxx"


Sprite::Sprite(std::string filename, Point isoCoordinates)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _texture = nullptr;

  loadTexture(filename, true);
  _isoCoordinates = isoCoordinates;
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates);
}

Sprite::~Sprite()
{
  SDL_DestroyTexture(_texture);
}

void Sprite::render()
{
  _zoomLevel = Resources::getZoomLevel();
  _cameraOffset = Resources::getCameraOffset();
  _screenCoordinates = Resources::convertIsoToScreenCoordinates(_isoCoordinates);

  //Render only whats visible
  const int offscreen_tolerance = 3 * TILE_SIZE * _zoomLevel;
  int screen_width;
  int screen_height;
  SDL_GetWindowSize(_window, &screen_width, &screen_height);

  if (( _screenCoordinates.getX() >= 0 - offscreen_tolerance )
  ||  ( _screenCoordinates.getX() + TILE_SIZE * _zoomLevel <= screen_width + offscreen_tolerance )
  ||  ( _screenCoordinates.getY() >= 0 - offscreen_tolerance )
  ||  ( _screenCoordinates.getY() + TILE_SIZE * _zoomLevel <= screen_height + offscreen_tolerance ))
  {
    // TODO: w and h parameter might change for sprites that consist of multiple tiles
    renderTexture(_screenCoordinates, TILE_SIZE * _zoomLevel, TILE_SIZE * _zoomLevel);
  }
}

void Sprite::setTileIsoCoordinates(Point isoCoords)
{
  _isoCoordinates = isoCoords;
}


// Texture Handling
void Sprite::loadTexture(std::string file, bool colorkey)
{
  SDL_Surface* loadedImage = IMG_Load(file.c_str());
  //_texture = nullptr

  if ( loadedImage != nullptr )
  {
    if ( colorkey )
    {
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0, 0xFF, 0xFF));	//Cyan.
    }
    _texture = SDL_CreateTextureFromSurface(_renderer, loadedImage);
    SDL_FreeSurface(loadedImage);
    if ( _texture == nullptr )
      printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
  }
  else
    printf("Error loading File %s", IMG_GetError());
}

void Sprite::renderTexture(Point tileScreenCoords, int w, int h)
{
  SDL_Rect dst;
  dst.x = _screenCoordinates.getX();
  dst.y = _screenCoordinates.getY();
  dst.w = w;
  dst.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &dst);
}

void Sprite::renderTexture(Point tileScreenCoords)
{
  int w, h;
  SDL_QueryTexture(_texture, NULL, NULL, &w, &h);
  renderTexture(_screenCoordinates, w, h);
}


void Sprite::changeSprite(std::string filename)
{
 // TODO: Implement texture streaming.
}