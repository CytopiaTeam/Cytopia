#ifndef CAMERA_HXX_
#define CAMERA_HXX_

#include "SDL.h"
#include "point.hxx"

//class Engine;

class Camera
{
public:
  /// the size of the default floor tiles.
  static SDL_Point tileSize;
  static SDL_Point cameraOffset;
  static float zoomLevel;

private:
  Camera() = delete;
  ~Camera() = delete;
};

#endif