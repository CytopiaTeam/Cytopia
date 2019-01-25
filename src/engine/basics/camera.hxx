#ifndef CAMERA_HXX_
#define CAMERA_HXX_

#include "SDL.h"
#include "point.hxx"

//class Engine;

class Camera
{
public:
  static float zoomLevel;

private:
  Camera() = delete;
  ~Camera() = delete;
};

#endif