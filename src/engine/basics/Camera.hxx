#ifndef CAMERA_HXX_
#define CAMERA_HXX_

#include "Point.hxx"
#include "../../util/Singleton.hxx"

#include <SDL.h>

class Camera : public Singleton<Camera>
{
  friend Singleton;

public:
  /**
  * @brief Centers camera on given isometric coordinates
  * @param isoCoordinates coordinates to center the camera on
  */
  void centerScreenOnPoint(const Point &isoCoordinates);

  /**
  * @brief Centers camera on the middle of the map
  */
  void centerScreenOnMapCenter();

  /**
  * @brief Increases the zoom level of the camera
  */
  void increaseZoomLevel();

  /** 
  * @brief Decreases the zoom level of the camera
  */
  void decreaseZoomLevel();

  inline void changeZoomLevel(bool inc) { inc ? increaseZoomLevel() : decreaseZoomLevel(); }

  /**
  * @brief Sets the pinch distance for touch screens
  * @param pinchDistance the pinch distance
  * @param isoX the x-center of the pinch
  * @param isoY the y-center of the pinch
  */
  void setPinchDistance(float pinchDistance, int isoX, int isoY);

  /**
  * @brief Move the camera in the given direction
  */
  void moveCamera(int xOffset, int yOffset);
  void canMove(bool move) { m_canMove = move; }

  const SDL_Point &cameraOffset() const noexcept;
  const double &zoomLevel() const noexcept;
  const SDL_Point &tileSize() const noexcept;

  void canScale(bool value) { m_canScale = value; }

private:
  Camera() = default;
  ~Camera() = default;

  float m_PinchDistance = 0.f;
  SDL_Point m_TileSize{32, 16};
  SDL_Point m_CameraOffset{0, 0};
  double m_ZoomLevel = 1.0; ///< the current zoom level of the camera
  bool m_canScale = true;
  bool m_canMove = true;
  Point m_CenterIsoCoordinates;
};

#endif
