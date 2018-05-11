#include "resources.hxx"

// Instantiate static variables
SDL_Renderer* Resources::_renderer = nullptr;
SDL_Window* Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;
const int Resources::_TILE_SIZE = 32;

Resources::Resources()
{

}

Resources::~Resources()
{

}

SDL_Renderer* Resources::getRenderer()
{
  return _renderer;
}

void Resources::setRenderer(SDL_Renderer* renderer)
{
  //SDL_Renderer* Resources::_renderer;
  _renderer = renderer;
}

SDL_Window* Resources::getWindow()
{
  return _window;
}

void Resources::setWindow(SDL_Window* window)
{
  _window = window;
}

Point Resources::getCameraOffset()
{
  return _cameraOffset;
}

void Resources::setCameraOffset(Point cameraOffset)
{
  _cameraOffset = cameraOffset;
}

float Resources::getZoomLevel()
{
  return _zoomLevel;
}

void Resources::setZoomLevel(float zoomLevel)
{
  _zoomLevel = zoomLevel;
}

int Resources::getTileSize()
{
  return _TILE_SIZE;
}

Point Resources::convertScreenToIsoCoordinates(Point screenCoordinates, bool calcWithoutOffset)
{
  // TODO: Bug: This does not work for tiles with increased height right now.
  int x, y;

  if (calcWithoutOffset)
  {
    x = (screenCoordinates.getX() + 2.0 * (screenCoordinates.getY())) / (_TILE_SIZE * _zoomLevel) - 1.5;
    y = (screenCoordinates.getX() - 2.0 * (screenCoordinates.getY())) / (_TILE_SIZE * _zoomLevel) + 1.5;
  }
  else
  {
    x = (screenCoordinates.getX() + _cameraOffset.getX() + 2.0 * (screenCoordinates.getY() + _cameraOffset.getY())) / (_TILE_SIZE * _zoomLevel) - 1.5;
    y = (screenCoordinates.getX() + _cameraOffset.getX() - 2.0 * (screenCoordinates.getY() + _cameraOffset.getY())) / (_TILE_SIZE * _zoomLevel) + 1.5;
  }
  return Point (x, y);
}

Point Resources::convertIsoToScreenCoordinates(Point isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.getHeight();
  int heightOffset = 20;

  if (calcWithoutOffset)
  {
    x = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.5);
    y = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.25) - (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.25);
  }
  else
  {
    x = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.5)  - _cameraOffset.getX();
    y = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.25) - (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.25) - _cameraOffset.getY();
  }
  
  if (height > 0)
  {
    y = (y - ((_TILE_SIZE - heightOffset) * height * _zoomLevel));
  }
  return Point (x, y);
}

