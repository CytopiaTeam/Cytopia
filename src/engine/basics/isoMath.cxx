#include "isoMath.hxx"

#include "../Engine.hxx"
#include "Camera.hxx"
#include "Settings.hxx"
#include "point.hxx"
#include "LOG.hxx"

Point calculateIsoCoordinates(const SDL_Point &screenCoordinates, int height)
{
   int isoX =
      static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x + 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
                           (Camera::tileSize.x * Camera::zoomLevel) +
                       1);
   int isoY =
      static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x - 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
                       (Camera::tileSize.x * Camera::zoomLevel));  
   //int isoY =
   //   static_cast<int>((screenCoordinates.x + Camera::cameraOffset.x - 2.0 * (screenCoordinates.y + Camera::cameraOffset.y)) /
   //                     (((Camera::tileSize.x - 26 ) * height) * Camera::zoomLevel));
   //y = std::round(static_cast<float>((y - ((Camera::tileSize.x - heightOffset) * isoCoordinates.height) * Camera::zoomLevel)));
   
   isoX = static_cast<int>(
      std::round(((static_cast<float>(screenCoordinates.x) + static_cast<float>(Camera::cameraOffset.x) +
                   2.0 * (static_cast<float>(screenCoordinates.y) + static_cast<float>(Camera::cameraOffset.y))) /
                      (static_cast<float>(Camera::tileSize.x) * Camera::zoomLevel) +
                  1)));
  

  isoY = static_cast<int>(std::round(((static_cast<float>(screenCoordinates.x) + static_cast<float>(Camera::cameraOffset.x) - 2.0 * (static_cast<float>(screenCoordinates.y) + static_cast<float>(Camera::cameraOffset.y))) /
                       (static_cast<float>(Camera::tileSize.x) * static_cast<float>(Camera::zoomLevel)))));

  return {isoX, isoY, 0, 0};
}

SDL_Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  const int heightOffset = 24;

  const int zoomedTileSizeX = static_cast<int>(std::round(static_cast<float>(Camera::tileSize.x) * Camera::zoomLevel));
  const int zoomedTileSizeY = static_cast<int>(std::round(static_cast<float>(Camera::tileSize.y) * Camera::zoomLevel));

  int x = static_cast<int>(std::round((static_cast<float>(zoomedTileSizeX) * static_cast<float>(isoCoordinates.x) + static_cast<float>(zoomedTileSizeX) * static_cast<float>(isoCoordinates.y)) / 2));
  int y = static_cast<int>(std::round((static_cast<float>(zoomedTileSizeY) * static_cast<float>(isoCoordinates.x) - static_cast<float>(zoomedTileSizeY) * static_cast<float>(isoCoordinates.y)) / 2));
  //LOG(LOG_INFO) << "x / y vorher " << x << ", " << y;
  if (!calcWithoutOffset)
  {
    x -= Camera::cameraOffset.x;
    y -= Camera::cameraOffset.y;
  }

  //LOG(LOG_INFO) << "x / y nach offset " << x << ", " << y;
  if (isoCoordinates.height > 0)
  {
    y = std::round(static_cast<float>((y - (((Camera::tileSize.x - heightOffset) * isoCoordinates.height) * Camera::zoomLevel))));
  }
  //LOG(LOG_INFO) << "x / y nach berechnung" << x << ", " << y;

  return {x, y};
}

Point convertScreenToIsoCoordinates(const SDL_Point &screenCoordinates)
{
  Point foundCoordinates = Point();
  if (Engine::instance().map != nullptr)
    foundCoordinates = Engine::instance().map->findNodeInMap(screenCoordinates);

  // if negative coordinates are returned, this means that the point is outside of the grid
  // calculate the coordinates instead and make sure it's within grid boundaries

    //foundCoordinates = calculateIsoCoordinates(screenCoordinates);
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
  
  return foundCoordinates;
}

bool isPointWithinMapBoundaries(const Point &isoCoordinates)
{
  return (isoCoordinates.x >= 0 && isoCoordinates.x < Settings::instance().mapSize) &&
         (isoCoordinates.y >= 0 && isoCoordinates.y < Settings::instance().mapSize);
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
