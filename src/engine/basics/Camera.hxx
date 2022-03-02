#ifndef CAMERA_HXX_
#define CAMERA_HXX_

#include "point.hxx"
#include "../../util/Singleton.hxx"

#include <SDL.h>

class Camera : public Singleton<Camera>
{
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

  /**
   * @brief Sets the pinch distance for Android
   * @param pinchDistance the pinch distance
   * @param isoX the x-center of the pinch
   * @param isoY the y-center of the pinch
   */
  void setPinchDistance(float pinchDistance, int isoX, int isoY);

  /**
   * @brief Sets the window
   * @todo  Remove this when NewUI is complete
   */
  void setWindow(class Window *);

  /**
   * @brief Sets the center ISO coordinates of the Camera
   */
  void setCenterIsoCoordinates(Point &&);
  
  /**
   * @brief Move the camera in the Y direction
   */
  void moveCameraY(float yOffset);
  
  /**
   * @brief Move the camera in the X direction
   */
  void moveCameraX(float xOffset);

  const SDL_Point & cameraOffset() const noexcept;
  const double & zoomLevel() const noexcept;
  const SDL_Point & tileSize() const noexcept;

private:
  Camera() = default;
  ~Camera();
  friend Singleton;
  
  float m_PinchDistance = 0.f;
  SDL_Point m_TileSize {32, 16};
  SDL_Point m_CameraOffset {0, 0};
  double m_ZoomLevel = 1.0;
  Point m_CenterIsoCoordinates;
  class Window * m_Window = nullptr;
};

#endif
