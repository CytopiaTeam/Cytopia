#ifndef RESOURCES_HXX_
#define RESOURCES_HXX_

#include "SDL2/SDL.h"
#include "point.hxx"

class Resources
{
public:
  static SDL_Renderer* getRenderer();
  static void setRenderer(SDL_Renderer* renderer);
  static SDL_Window* getWindow();
  static void setWindow(SDL_Window* window);

  static float getZoomLevel();
  static void setZoomLevel(float zoomLevel);
  static Point getCameraOffset();
  static void setCameraOffset(Point cameraOffset);

private:
  Resources();
  ~Resources();

  static SDL_Renderer* _renderer;
  static SDL_Window* _window;

  static float _zoomLevel;
  static Point _cameraOffset;
};

#endif