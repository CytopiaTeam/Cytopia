#include "PointFunctions.hxx"
static std::vector<Point> getLine;

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