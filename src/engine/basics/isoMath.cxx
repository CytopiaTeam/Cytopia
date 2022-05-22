#include "isoMath.hxx"

#include "Camera.hxx"
#include "Settings.hxx"
#include "Point.hxx"
#include "LOG.hxx"
#include <MapFunctions.hxx>

bool reverseDirection = false;

Point calculateIsoCoordinates(const SDL_Point &screenCoordinates)
{
  const SDL_Point &cameraOffset = Camera::instance().cameraOffset();
  const double &zoomLevel = Camera::instance().zoomLevel();
  const SDL_Point &tileSize = Camera::instance().tileSize();

  const int isoX = static_cast<int>(
      (screenCoordinates.x + cameraOffset.x + 2.0 * (screenCoordinates.y + cameraOffset.y)) / (tileSize.x * zoomLevel) + 1);
  const int isoY = static_cast<int>((screenCoordinates.x + cameraOffset.x - 2.0 * (screenCoordinates.y + cameraOffset.y)) /
                                    (tileSize.x * zoomLevel));

  return {isoX, isoY, 0, 0};
}

SDL_Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  const int heightOffset = 24;
  const SDL_Point &cameraOffset = Camera::instance().cameraOffset();
  const double &zoomLevel = Camera::instance().zoomLevel();
  const SDL_Point &tileSize = Camera::instance().tileSize();

  int x = static_cast<int>(std::round(static_cast<double>((isoCoordinates.x + isoCoordinates.y) * tileSize.x) * zoomLevel) / 2);
  int y = static_cast<int>(std::round(static_cast<double>((isoCoordinates.x - isoCoordinates.y) * tileSize.y) * zoomLevel) / 2);

  if (!calcWithoutOffset)
  {
    x -= cameraOffset.x;
    y -= cameraOffset.y;
  }

  if (isoCoordinates.height > 0)
  {
    y -= static_cast<int>(std::round(static_cast<double>((tileSize.x - heightOffset) * isoCoordinates.height) * zoomLevel));
  }

  return {x, y};
}

Point convertScreenToIsoCoordinates(const SDL_Point &screenCoordinates)
{
  Point foundCoordinates = Point();
  foundCoordinates = MapFunctions::instance().findNodeInMap(screenCoordinates);

  // if negative coordinates are returned, this means that the point is outside of the grid
  // calculate the coordinates instead and make sure it's within grid boundaries
  if (foundCoordinates.x == -1)
  {
    foundCoordinates = calculateIsoCoordinates(screenCoordinates);
    if (foundCoordinates.x < 0)
    {
      foundCoordinates.x = 0;
    }
    else if (foundCoordinates.x >= Settings::instance().mapSize)
    {
      // map (vector) size is 128, but coordinates ranges from 0 - 127
      foundCoordinates.x = Settings::instance().mapSize - 1;
    }
    if (foundCoordinates.y < 0)
    {
      foundCoordinates.y = 0;
    }
    else if (foundCoordinates.y >= Settings::instance().mapSize)
    {
      // map (vector) size is 128, but coordinates range from 0 - 127
      foundCoordinates.y = Settings::instance().mapSize - 1;
    }
  }
  return foundCoordinates;
}

bool isPointWithinMapBoundaries(const std::vector<Point> &isoCoordinates)
{
  for (auto p : isoCoordinates)
  {
    if (!(p.isWithinMapBoundaries()))
    {
      return false;
    }
  }
  return true;
}