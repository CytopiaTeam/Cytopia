#include "isoMath.hxx"

#include "../engine.hxx"
#include "camera.hxx"
#include "settings.hxx"
#include "point.hxx"

Point calculateIsoCoordinates(const Point &screenCoordinates)
{
  int isoX =
      static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x + 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
                           (Camera::tileSize.x * Camera::zoomLevel) +
                       1);
  int isoY =
      static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x - 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
                       (Camera::tileSize.x * Camera::zoomLevel));

  return {isoX, isoY, 0, 0};
}

Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.height;
  int heightOffset = 18;

  if (calcWithoutOffset)
  {
    x = static_cast<int>((Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.x * 0.5) +
                         (Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.y * 0.5));

    y = static_cast<int>((Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.x * 0.25) -
                         (Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.y * 0.25));
  }
  else
  {
    x = static_cast<int>((Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.x * 0.5) +
                         (Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.y * 0.5) - Camera::cameraOffset.x);
    y = static_cast<int>((Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.x * 0.25) -
                         (Camera::tileSize.x * Camera::zoomLevel * isoCoordinates.y * 0.25) - Camera::cameraOffset.y);
  }

  if (height > 0)
  {
    y = static_cast<int>(y - ((Camera::tileSize.x - heightOffset) * height * Camera::zoomLevel));
  }

  return {x, y, 0, 0};
}

Point convertScreenToIsoCoordinates(const Point &screenCoordinates)
{
  return Engine::instance().findNodeInMap(screenCoordinates);
}

bool isPointWithinMapBoundaries(const Point &isoCoordinates)
{
  return (isoCoordinates.x >= 0 && isoCoordinates.x <= Settings::instance().settings.mapSize) &&
         (isoCoordinates.y >= 0 && isoCoordinates.y <= Settings::instance().settings.mapSize);
}