#include "isoMath.hxx"

#include "../Engine.hxx"
#include "Camera.hxx"
#include "Settings.hxx"
#include "point.hxx"

Point calculateIsoCoordinates(const SDL_Point &screenCoordinates)
{
  const int isoX =
      static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x + 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
                           (Camera::tileSize.x * Camera::zoomLevel) +
                       1);
  const int isoY =
      static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x - 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
                       (Camera::tileSize.x * Camera::zoomLevel));

  return {isoX, isoY, 0, 0};
}

SDL_Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  const int heightOffset = 24;

  const int zoomedTileSizeX = static_cast<int>(Camera::tileSize.x * Camera::zoomLevel);
  const int zoomedTileSizeY = static_cast<int>(Camera::tileSize.y * Camera::zoomLevel);

  int x = static_cast<int>(zoomedTileSizeX * isoCoordinates.x + zoomedTileSizeX * isoCoordinates.y) / 2;
  int y = static_cast<int>(zoomedTileSizeY * isoCoordinates.x - zoomedTileSizeY * isoCoordinates.y) / 2;

  if (!calcWithoutOffset)
  {
    x -= Camera::cameraOffset.x;
    y -= Camera::cameraOffset.y;
  }

  if (isoCoordinates.height > 0)
  {
    y = static_cast<int>(y - ((Camera::tileSize.x - heightOffset) * isoCoordinates.height * Camera::zoomLevel));
  }

  return {x, y};
}

Point convertScreenToIsoCoordinates(const SDL_Point &screenCoordinates)
{
  Point foundCoordinates = Engine::instance().map->findNodeInMap(screenCoordinates);

  // if negative coordinates are returned, this means that the point is outside of the grid
  // calculate the coordinates instead and make sure it's within grid boundaries
  if (foundCoordinates.x == -1)
  {
    foundCoordinates = calculateIsoCoordinates(screenCoordinates);
    if (foundCoordinates.x < 0)
    {
      foundCoordinates.x = 0;
    }
    else if (foundCoordinates.x >= Settings::instance().settings.mapSize)
    {
      // map (vector) size is 128, but coordinates ranges from 0 - 127
      foundCoordinates.x = Settings::instance().settings.mapSize - 1;
    }
    if (foundCoordinates.y < 0)
    {
      foundCoordinates.y = 0;
    }
    else if (foundCoordinates.y >= Settings::instance().settings.mapSize)
    {
      // map (vector) size is 128, but coordinates range from 0 - 127
      foundCoordinates.y = Settings::instance().settings.mapSize - 1;
    }
  }
  return foundCoordinates;
}

bool isPointWithinMapBoundaries(const Point &isoCoordinates)
{
  return (isoCoordinates.x >= 0 && isoCoordinates.x < Settings::instance().settings.mapSize) &&
         (isoCoordinates.y >= 0 && isoCoordinates.y < Settings::instance().settings.mapSize);
}