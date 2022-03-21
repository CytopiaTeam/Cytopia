#include "isoMath.hxx"

#include "../Engine.hxx"
#include "Camera.hxx"
#include "Settings.hxx"
#include "point.hxx"
#include "LOG.hxx"

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
  if (Engine::instance().map != nullptr)
    foundCoordinates = Engine::instance().map->findNodeInMap(screenCoordinates);

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

std::vector<Point> getRectangleSelectionNodes(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd)
{
  std::vector<Point> rectangle;
  SDL_Point startRect;
  SDL_Point endRect;
  std::tie(startRect.x, endRect.x) = std::minmax(isoCoordinatesStart.x, isoCoordinatesEnd.x);
  std::tie(startRect.y, endRect.y) = std::minmax(isoCoordinatesStart.y, isoCoordinatesEnd.y);

  for (int x = startRect.x; x <= endRect.x; x++)
  {
    for (int y = startRect.y; y <= endRect.y; y++)
    {
      rectangle.push_back(Point{x, y});
    }
  }

  return rectangle;
}

std::vector<Point> getRectangularLineSelectionNodes(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd)
{
  std::vector<Point> rectangle;
  int directionX = isoCoordinatesStart.x < isoCoordinatesEnd.x ? 1 : -1;
  int directionY = isoCoordinatesStart.y < isoCoordinatesEnd.y ? 1 : -1;
  int staticX, staticY;
  SDL_Point startRect;
  SDL_Point endRect;

  uint32_t xDist = std::abs(isoCoordinatesStart.x - isoCoordinatesEnd.x);
  uint32_t yDist = std::abs(isoCoordinatesStart.y - isoCoordinatesEnd.y);

  if (xDist == 0 && yDist == 1)
  {
    reverseDirection = true;
  }
  else if (xDist == 1 && yDist == 0)
  {
    reverseDirection = false;
  }
  else if (xDist == 0 && yDist == 0)
  {
    rectangle.push_back(Point{isoCoordinatesStart.x, isoCoordinatesStart.y});
  }

  if (reverseDirection)
  {
    staticX = isoCoordinatesStart.x;
    staticY = isoCoordinatesEnd.y;
  }
  else
  {
    staticX = isoCoordinatesEnd.x;
    staticY = isoCoordinatesStart.y;
  }

  for (int x = isoCoordinatesStart.x; x != isoCoordinatesEnd.x; x += directionX)
  {
    rectangle.push_back(Point{x, staticY});
  }

  for (int y = isoCoordinatesStart.y; y != isoCoordinatesEnd.y; y += directionY)
  {
    rectangle.push_back(Point{staticX, y});
  }

  return rectangle;
}
