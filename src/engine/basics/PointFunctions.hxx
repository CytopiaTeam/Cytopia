#ifndef POINTFUNCTIONS_HXX_
#define POINTFUNCTIONS_HXX_

#include "Point.hxx"
#include "../../util/LOG.hxx"
#include <vector>

class PointFunctions
{
public:
  /** \brief Creates a line between two points using the Bresenham Line algorithm
* Creates a line between two points using the Bresenham Line algorithm
* @param Point() - start coordinates
* @param Point() - end coordinates
* @return std::vector<Point>() - contains coordinates for each tile between start and end coordinates, including start and end
*/
  static std::vector<Point> getLine(Point isoCoordinatesStart, Point end);
};

#endif