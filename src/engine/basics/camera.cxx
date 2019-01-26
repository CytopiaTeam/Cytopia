#include "camera.hxx"
#include "isoMath.hxx"
#include "settings.hxx"
#include "../engine.hxx"

SDL_Point Camera::tileSize = {32, 16};
SDL_Point Camera::cameraOffset = {0, 0};
Point Camera::centerIsoCoordinates;
float Camera::zoomLevel = 1.0f;

void Camera::increaseZoomLevel()
{
  if (zoomLevel < 4.0f)
  {
    zoomLevel += 0.25f;
    centerScreenOnPoint(centerIsoCoordinates);
    Engine::instance().getMap()->refresh();
  }
}

void Camera::decreaseZoomLevel()
{
  if (zoomLevel > 0.5f)
  {
    zoomLevel -= 0.25f;
    centerScreenOnPoint(centerIsoCoordinates);
    Engine::instance().getMap()->refresh();
  }
}

void Camera::centerScreenOnPoint(const Point &isoCoordinates)
{
  if (isPointWithinMapBoundaries(isoCoordinates))
  {
    centerIsoCoordinates = isoCoordinates;
    SDL_Point screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates, true);
    int x, y;

    x = static_cast<int>((screenCoordinates.x + (tileSize.x * zoomLevel) * 0.5) -
                         Settings::instance().settings.screenWidth * 0.5);
    y = static_cast<int>((screenCoordinates.y + (tileSize.x * zoomLevel) * 0.75) -
                         Settings::instance().settings.screenHeight * 0.5);

    cameraOffset = {x, y};
    Engine::instance().getMap()->refresh();
  }
}

void Camera::centerScreenOnMapCenter()
{
  centerIsoCoordinates = {Settings::instance().settings.mapSize / 2, Settings::instance().settings.mapSize / 2, 0, 0};
  centerScreenOnPoint(centerIsoCoordinates);
}