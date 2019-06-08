#include "Camera.hxx"
#include "isoMath.hxx"
#include "Settings.hxx"
#include "../Engine.hxx"

SDL_Point Camera::tileSize{32, 16};
SDL_Point Camera::cameraOffset{0, 0};
Point Camera::centerIsoCoordinates;
float Camera::zoomLevel = 1.0f;
float Camera::m_pinchDistance = 0.0f;

void Camera::increaseZoomLevel()
{
  if (zoomLevel < 4.0f)
  {
    zoomLevel += 0.5f;
    centerScreenOnPoint(centerIsoCoordinates);
    if (Engine::instance().map != nullptr)
      Engine::instance().map->refresh();
  }
}

void Camera::decreaseZoomLevel()
{
  if (zoomLevel > 0.5f)
  {
    zoomLevel -= 0.5f;
    centerScreenOnPoint(centerIsoCoordinates);
    if (Engine::instance().map != nullptr)
      Engine::instance().map->refresh();
  }
}

void Camera::setPinchDistance(float pinchDistance, int isoX, int isoY)
{
  m_pinchDistance += pinchDistance;

  if (m_pinchDistance > 0.25f)
  {
    m_pinchDistance = 0;
    if (zoomLevel < 4.0f)
    {
      centerScreenOnPoint({isoX, isoY, 0, 0});
    }
    increaseZoomLevel();
  }
  else if (m_pinchDistance < -0.25f)
  {
    m_pinchDistance = 0;
    if (zoomLevel > 0.5f)
    {
      centerScreenOnPoint({isoX, isoY, 0, 0});
    }
    decreaseZoomLevel();
  }
}

void Camera::centerScreenOnPoint(const Point &isoCoordinates)
{
  if (isPointWithinMapBoundaries(isoCoordinates))
  {
    centerIsoCoordinates = isoCoordinates;
    const SDL_Point screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates, true);

    int x = static_cast<int>((screenCoordinates.x + (tileSize.x * zoomLevel) * 0.5) - Settings::instance().screenWidth * 0.5);
    int y = static_cast<int>((screenCoordinates.y + (tileSize.y * zoomLevel) * 0.25) - Settings::instance().screenHeight * 0.5);

    x -= static_cast<int>((tileSize.x * zoomLevel) * 0.75);
    y -= static_cast<int>(tileSize.y * zoomLevel);

    cameraOffset = {x, y};
    if (Engine::instance().map != nullptr)
      Engine::instance().map->refresh();
  }
}

void Camera::centerScreenOnMapCenter()
{
  centerIsoCoordinates = {Settings::instance().mapSize / 2, Settings::instance().mapSize / 2, 0, 0};
  centerScreenOnPoint(centerIsoCoordinates);
}
