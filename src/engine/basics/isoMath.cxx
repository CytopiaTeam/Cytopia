#include "isoMath.hxx"

#include "../Engine.hxx"
#include "Camera.hxx"
#include "Settings.hxx"
#include "point.hxx"
#include "LOG.hxx"

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

  int x = static_cast<int>(
      std::round(static_cast<double>((isoCoordinates.x + isoCoordinates.y) * Camera::tileSize.x) * Camera::zoomLevel) / 2);
  int y = static_cast<int>(
      std::round(static_cast<double>((isoCoordinates.x - isoCoordinates.y) * Camera::tileSize.y) * Camera::zoomLevel) / 2);

  if (!calcWithoutOffset)
  {
    x -= Camera::cameraOffset.x;
    y -= Camera::cameraOffset.y;
  }

  if (isoCoordinates.height > 0)
  {
    y -= static_cast<int>(
        std::round(static_cast<double>((Camera::tileSize.x - heightOffset) * isoCoordinates.height) * Camera::zoomLevel));
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

bool isPointWithinMapBoundaries(int x, int y)
{
  return (x >= 0 && x < Settings::instance().mapSize) && (y >= 0 && y < Settings::instance().mapSize);
}

bool isPointWithinMapBoundaries(const Point &isoCoordinates)
{
  return (isoCoordinates.x >= 0 && isoCoordinates.x < Settings::instance().mapSize) &&
         (isoCoordinates.y >= 0 && isoCoordinates.y < Settings::instance().mapSize);
}

bool isPointWithinMapBoundaries(const std::vector<Point> &isoCoordinates)
{
  for (auto p : isoCoordinates)
  {
    if (!isPointWithinMapBoundaries(p))
    {
      return false;
    }
  }
  return true;
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
  SDL_Point startRect;
  SDL_Point endRect;
  bool reverseDirection = false;

  std::tie(startRect.x, endRect.x) = std::minmax(isoCoordinatesStart.x, isoCoordinatesEnd.x);
  std::tie(startRect.y, endRect.y) = std::minmax(isoCoordinatesStart.y, isoCoordinatesEnd.y);

  if ((isoCoordinatesStart.x > isoCoordinatesEnd.x && isoCoordinatesStart.y < isoCoordinatesEnd.y) ||
      (isoCoordinatesStart.x < isoCoordinatesEnd.x && isoCoordinatesStart.y > isoCoordinatesEnd.y))
  {
    reverseDirection = true;
  }
  for (int x = startRect.x; x <= endRect.x; x++)
  {
    for (int y = startRect.y; y <= endRect.y; y++)
    {
      if (reverseDirection)
      {
        if (x == startRect.x || y == startRect.y)
        {
          rectangle.push_back(Point{x, y});
        }
      }
      else
      {
        if (x == startRect.x || y == endRect.y)
        {
          rectangle.push_back(Point{x, y});
        }
      }
    }
  }

  return rectangle;
}

std::vector<Point> createBresenhamLine(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd)
{
  std::vector<Point> line;
  int x0 = isoCoordinatesStart.x;
  int x1 = isoCoordinatesEnd.x;
  int y0 = isoCoordinatesStart.y;
  int y1 = isoCoordinatesEnd.y;

  Point p;
  p.x = x0;
  p.y = y0;
  line.push_back(p);

  if (x0 != x1 || y0 != y1)
  {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int stepx = 0;
    int stepy = 0;

    if (dx < 0)
    {
      dx = -dx;
      stepx = -1;
    }
    else
    {
      stepx = 1;
    }

    if (dy < 0)
    {
      dy = -dy;
      stepy = -1;
    }
    else
    {
      stepy = 1;
    }

    // dx is now 2*dx
    dx <<= 1;
    // dy is now 2*dy
    dy <<= 1;

    if (dx > dy)
    {
      int fraction = dy - (dx >> 1);
      while (x0 != x1)
      {
        x0 += stepx;
        if (fraction >= 0)
        {
          p.x = x0;
          p.y = y0;
          line.push_back(p);

          y0 += stepy;
          fraction -= dx;
        }
        fraction += dy;
        if (0 <= x0 && 0 <= y0)
        {
          p.x = x0;
          p.y = y0;
          line.push_back(p);
        }
      }
    }
    else
    {
      int fraction = dx - (dy >> 1);
      while (y0 != y1)
      {
        if (fraction >= 0)
        {
          x0 += stepx;
          fraction -= dy;

          p.x = x0;
          p.y = y0;
          line.push_back(p);
        }
        y0 += stepy;
        fraction += dx;
        if (0 <= x0 && 0 <= y0)
        {
          p.x = x0;
          p.y = y0;
          line.push_back(p);
        }
      }
    }
  }

  return line;
}
