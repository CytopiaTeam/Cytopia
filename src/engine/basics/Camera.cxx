#include "Camera.hxx"
#include "isoMath.hxx"
#include "Settings.hxx"
#include "../Engine.hxx"
#include "../../view/Window.hxx"
#include "../../util/Exception.hxx"
#include "../../util/LOG.hxx"

void Camera::increaseZoomLevel()
{
  if (m_ZoomLevel < 4.0)
  {
    m_ZoomLevel += 0.5;
    centerScreenOnPoint(m_CenterIsoCoordinates);
    if (Engine::instance().map != nullptr) {
      Engine::instance().map->refresh();
    }
  }
}

void Camera::decreaseZoomLevel()
{
  if (m_ZoomLevel > 0.5)
  {
    m_ZoomLevel -= 0.5;
    centerScreenOnPoint(m_CenterIsoCoordinates);
    if (Engine::instance().map != nullptr) {
      Engine::instance().map->refresh();
    }
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
  if (isPointWithinMapBoundaries(isoCoordinates))
  {
    m_CenterIsoCoordinates = isoCoordinates;
    const SDL_Point screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates, true);
    if(!m_Window) {
      throw CytopiaError{TRACE_INFO "Cannot center screen on a point without a Window"};
    }
    int screenWidth = m_Window->getBounds().width(); 
    int screenHeight = m_Window->getBounds().height(); 

    int x = static_cast<int>((screenCoordinates.x + (m_TileSize.x * m_ZoomLevel) * 0.5) - screenWidth * 0.5);
    int y = static_cast<int>((screenCoordinates.y + (m_TileSize.y * m_ZoomLevel) * 0.25) - screenHeight * 0.5);

    x -= static_cast<int>((m_TileSize.x * m_ZoomLevel) * 0.75);
    y -= static_cast<int>(m_TileSize.y * m_ZoomLevel);

    m_CameraOffset = {x, y};
    if (Engine::instance().map != nullptr) {
      Engine::instance().map->refresh();
    }
  }
}

void Camera::centerScreenOnMapCenter()
{
  m_CenterIsoCoordinates = {Settings::instance().mapSize / 2, Settings::instance().mapSize / 2, 0, 0};
  centerScreenOnPoint(m_CenterIsoCoordinates);
}

void Camera::setWindow(Window * window) {
  m_Window = window;
}

void Camera::setCenterIsoCoordinates(Point && p) {
  std::swap(m_CenterIsoCoordinates, p);
}

void Camera::moveCameraY(float yOffset) {
  m_CameraOffset.y -= yOffset;
}

void Camera::moveCameraX(float xOffset) {
  m_CameraOffset.x -= xOffset;
}

const SDL_Point & Camera::cameraOffset() const noexcept
{
  return m_CameraOffset;
}

const double & Camera::zoomLevel() const noexcept
{
  return m_ZoomLevel;
}

const SDL_Point & Camera::tileSize() const noexcept
{
  return m_TileSize;
}

Camera::~Camera()
{
  LOG(LOG_INFO) << "Destroying Camera";
}
