#include "PointFunctions.hxx"
#include <cassert>

// forward declarations
static std::vector<Point> getLine;
static std::vector<Point> getStraightLine;
static std::vector<Point> getArea;
static std::vector<Point> getNeighbors;

std::vector<Point> PointFunctions::getLine(Point isoCoordinatesStart, Point isoCoordinatesEnd)
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

std::vector<Point> PointFunctions::getStraightLine(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd)
{
  std::vector<Point> rectangle;
  int directionX = isoCoordinatesStart.x < isoCoordinatesEnd.x ? 1 : -1;
  int directionY = isoCoordinatesStart.y < isoCoordinatesEnd.y ? 1 : -1;
  int staticX, staticY;
  bool reverseDirection = false;

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

std::vector<Point> PointFunctions::getArea(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd)
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

std::vector<Point> PointFunctions::getNeighbors(const Point &isoCoordinates, const bool includeCentralNode, int distance)
{
  std::vector<Point> neighbors;

  for (int xOffset = -distance; xOffset <= distance; ++xOffset)
  {
    for (int yOffset = -distance; yOffset <= distance; ++yOffset)
    {
      if (!includeCentralNode && (xOffset == 0) && (yOffset == 0))
      {
        continue;
      }

      Point neighbor;
      neighbor.x = isoCoordinates.x + xOffset;
      neighbor.y = isoCoordinates.y + yOffset;

      if (neighbor.isWithinMapBoundaries())
      {
        neighbors.push_back(neighbor);
      }
    }
  }

  return neighbors;
}

NeighborNodesPosition PointFunctions::getNeighborPositionToOrigin(const Point &neighboringPoint, const Point &originPoint)
{
  int neighborOffsetX = neighboringPoint.x - originPoint.x;
  int neighborOffsetY = neighboringPoint.y - originPoint.y;

  switch (neighborOffsetX)
  {
  case 0:
    switch (neighborOffsetY)
    {
    case 0:
      return NeighborNodesPosition::CENTER;
    case 1:
      return NeighborNodesPosition::TOP;
    case -1:
      return NeighborNodesPosition::BOTTOM;
    }
    break;
  case 1:
    switch (neighborOffsetY)
    {
    case 0:
      return NeighborNodesPosition::RIGHT;
    case 1:
      return NeighborNodesPosition::TOP_RIGHT;
    case -1:
      return NeighborNodesPosition::BOTTOM_RIGHT;
    }
    break;
  case -1:
    switch (neighborOffsetY)
    {
    case 0:
      return NeighborNodesPosition::LEFT;
    case 1:
      return NeighborNodesPosition::TOP_LEFT;
    case -1:
      return NeighborNodesPosition::BOTTOM_LEFT;
    }
    break;
  }
  assert(false); // this should never happen!
  return NeighborNodesPosition::CENTER;
}
