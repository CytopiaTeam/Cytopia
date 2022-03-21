#ifndef POINTFUNCTIONS_HXX_
#define POINTFUNCTIONS_HXX_

#include "point.hxx"
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

  /** \brief Gets all nodes in a straight line from start and end point
* Gets all nodes in a straight line between start and end point
* @param Point() - start coordinates
* @param Point() - end coordinates
* @return std::vector<Point>() - contains coordinates for each tile between start and end coordinates, including start and end
*/
  static std::vector<Point> getStraightLine(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);

  /** \brief Gets all nodes in a rectangular area between start and end point
* Gets all nodes in a rectangle between start and end point
* @param Point() - start coordinates
* @param Point() - end coordinates
* @return std::vector<Point>() - contains coordinates for each tile between start and end coordinates, including start and end
*/
  static std::vector<Point> getArea(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);
};

#endif