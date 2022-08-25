#include "Camera.hxx"
#include "isoMath.hxx"
#include "Settings.hxx"
#include <MapFunctions.hxx>

void Camera::increaseZoomLevel()
{
  if (!m_canScale)
    return;

  if (m_ZoomLevel < 4.0)
  {
    m_ZoomLevel += 0.5;
    centerScreenOnPoint(m_CenterIsoCoordinates);
    MapFunctions::instance().refreshVisibleMap();
  }
}

void Camera::decreaseZoomLevel()
{
  if (!m_canScale)
    return;

  if (m_ZoomLevel > 0.5)
  {
    m_ZoomLevel -= 0.5;
    centerScreenOnPoint(m_CenterIsoCoordinates);
    MapFunctions::instance().refreshVisibleMap();
  }
}

void Camera::setPinchDistance(float pinchDistance, int isoX, int isoY)
{
  m_PinchDistance += pinchDistance;

  if (m_PinchDistance > 0.25F)
  {
    m_PinchDistance = 0;
    if (m_ZoomLevel < 4.0)
    {
      centerScreenOnPoint({isoX, isoY, 0, 0});
    }
    increaseZoomLevel();
  }
  else if (m_PinchDistance < -0.25F)
  {
    m_PinchDistance = 0;
    if (m_ZoomLevel > 0.5)
    {
      centerScreenOnPoint({isoX, isoY, 0, 0});
    }
    decreaseZoomLevel();
  }
}

void Camera::centerScreenOnPoint(const Point &isoCoordinates)
{
  if (isoCoordinates.isWithinMapBoundaries())
  {
    m_CenterIsoCoordinates = isoCoordinates;
    const SDL_Point screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates, true);

    int x = static_cast<int>((screenCoordinates.x + (m_TileSize.x * m_ZoomLevel) * 0.5) - Settings::instance().screenWidth * 0.5);
    int y =
        static_cast<int>((screenCoordinates.y + (m_TileSize.y * m_ZoomLevel) * 0.25) - Settings::instance().screenHeight * 0.5);

    x -= static_cast<int>((m_TileSize.x * m_ZoomLevel) * 0.75);
    y -= static_cast<int>(m_TileSize.y * m_ZoomLevel);

    m_CameraOffset = {x, y};
    MapFunctions::instance().refreshVisibleMap();
  }
}

void Camera::centerScreenOnMapCenter()
{
  m_CenterIsoCoordinates = {Settings::instance().mapSize / 2, Settings::instance().mapSize / 2, 0, 0};
  centerScreenOnPoint(m_CenterIsoCoordinates);
  MapFunctions::instance().refreshVisibleMap();
}

void Camera::moveCamera(int xOffset, int yOffset)
{
  if (!m_canMove)
    return;

  m_CameraOffset.x -= xOffset;
  m_CameraOffset.y -= yOffset;
  MapFunctions::instance().refreshVisibleMap();
  // update center coordinates
  m_CenterIsoCoordinates =
      convertScreenToIsoCoordinates({Settings::instance().screenWidth / 2, Settings::instance().screenHeight / 2});
}

const SDL_Point &Camera::cameraOffset() const noexcept { return m_CameraOffset; }

const double &Camera::zoomLevel() const noexcept { return m_ZoomLevel; }

const SDL_Point &Camera::tileSize() const noexcept { return m_TileSize; }
