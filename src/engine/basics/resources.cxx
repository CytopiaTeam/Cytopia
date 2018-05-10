#include "resources.hxx"

// Instantiate static variables
SDL_Renderer* Resources::_renderer = nullptr;
SDL_Window* Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;


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