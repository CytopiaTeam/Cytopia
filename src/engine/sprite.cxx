#include "sprite.hxx"


Sprite::Sprite(std::string filename, Point isoCoords)
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  
  loadTexture(filename, true);
  _isoCoords = isoCoords;
}

Sprite::~Sprite()
{

}

void Sprite::render()
{
  _zoomLevel = Resources::getZoomLevel();
  _cameraOffset = Resources::getCameraOffset();
  
  Point tileScreenCoords = getTileScreenCoordinates();

  if (_isoCoords.getHeight() > 0)
  {
    tileScreenCoords.setY(tileScreenCoords.getY() - ((TILE_SIZE - _heightOffset) * _isoCoords.getHeight())*_zoomLevel); 
  }

  //Render only whats visible
  const int offscreen_tolerance = 3 * TILE_SIZE * _zoomLevel;
  int screen_width;
  int screen_height;
  SDL_GetWindowSize(_window, &screen_width, &screen_height);

  if (( tileScreenCoords.getX() >= 0 - offscreen_tolerance ) 
  ||  ( tileScreenCoords.getX() + TILE_SIZE * _zoomLevel <= screen_width + offscreen_tolerance ) 
  ||  ( tileScreenCoords.getY() >= 0 - offscreen_tolerance ) 
  ||  ( tileScreenCoords.getY() + TILE_SIZE * _zoomLevel <= screen_height + offscreen_tolerance ))
  {
    // TODO: w and h parameter might change for sprites that consist of multiple tiles
    renderTexture(tileScreenCoords, TILE_SIZE * _zoomLevel, TILE_SIZE * _zoomLevel);
  }
}

Point Sprite::getTileScreenCoordinates()
{
  Point tileScreenCoords;
  int x, y;
  _zoomLevel = Resources::getZoomLevel();
  _cameraOffset = Resources::getCameraOffset();

  x = (TILE_SIZE * _zoomLevel * _isoCoords.getX() * 0.5) + (TILE_SIZE * _zoomLevel * _isoCoords.getY() * 0.5) - _cameraOffset.getX();
  y = ((TILE_SIZE * _zoomLevel * _isoCoords.getX() * 0.25) - (TILE_SIZE * _zoomLevel * _isoCoords.getY() * 0.25)) - _cameraOffset.getY();

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


// Texture Handling
void Sprite::loadTexture(std::string file, bool colorkey)
{
  SDL_Surface* loadedImage = IMG_Load(file.c_str());
  _texture = nullptr;

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
  dst.x = tileScreenCoords.getX();
  dst.y = tileScreenCoords.getY();
  dst.w = w;
  dst.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &dst);
}

void Sprite::renderTexture(Point tileScreenCoords)
{
  int w, h;
  SDL_QueryTexture(_texture, NULL, NULL, &w, &h);
  renderTexture(tileScreenCoords, w, h);
}
