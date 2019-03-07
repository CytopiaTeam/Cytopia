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

  /** \brief Centers camera on given isometric coordinates
  * Centers the camera on the given isometric coordinates.
  * @param Point object - coordinates to center the camera on
  */
  static void centerScreenOnPoint(const Point &isoCoordinates);

  /** \brief Centers camera on the middle of the map
  * Centers the camera on the middle of the map.
  */
  static void centerScreenOnMapCenter();

  static void increaseZoomLevel();
  static void decreaseZoomLevel();

  static Point centerIsoCoordinates;

private:
  Camera() = delete;
  ~Camera() = delete;

};

#endif