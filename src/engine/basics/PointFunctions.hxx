#ifndef POINTFUNCTIONS_HXX_
#define POINTFUNCTIONS_HXX_

#include "Point.hxx"
#include "../../util/LOG.hxx"
#include "betterEnums.hxx"
#include <vector>

BETTER_ENUM(NeighborNodesPosition, unsigned char, BOTTOM_LEFT = 1U << 6, LEFT = 1U << 2, TOP_LEFT = 1U << 4, BOTTOM = 1U << 1,
            CENTER = 0U, TOP = 1U, BOTTOM_RIGHT = 1U << 7, RIGHT = 1U << 3, TOP_RIGHT = 1U << 5);

class PointFunctions
{
public:
  /** \brief Creates a line between two points using the Bresenham Line algorithm
* @param isoCoordinatesStart start coordinates
* @param isoCoordinatesEnd end coordinates
* @return std::vector<Point>() containing coordinates for each tile between start and end coordinates, including start and end
*/
  static std::vector<Point> getLine(Point isoCoordinatesStart, Point isoCoordinatesEnd);

  /** \brief Gets all nodes in a straight line from start and end point
* @param isoCoordinatesStart start coordinates
* @param isoCoordinatesEnd end coordinates
* @return std::vector<Point>() containing coordinates for each tile between start and end coordinates, including start and end
*/
  static std::vector<Point> getStraightLine(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);

  /** \brief Gets all nodes in a rectangular area between start and end point
* @param isoCoordinatesStart start coordinates
* @param isoCoordinatesEnd end coordinates
* @return std::vector<Point>() containing coordinates for each tile between start and end coordinates, including start and end
*/
  static std::vector<Point> getArea(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);

  /** \brief Get all neighboring coordinates from provided map node isocoordinate.
  * @param isoCoordinates point to get neighbors of
  * @param includeCentralNode if set to true include the central node in the result.
  * @return std::vector<Point>() of all neighboring node coordinates.
  */
  static std::vector<Point> getNeighbors(const Point &isoCoordinates, const bool includeCentralNode, int distance = 1);

  /** \brief Get the position of the neighboring node to the originpoint (center of the neighborgroup).
  * @param neighboringPoint the neighboring point
  * @param originPoint the center of the neighbor group we want to get the direction to
  * @return An enum representing the Direction from the neighboring point to the origin point

  */
  static NeighborNodesPosition getNeighborPositionToOrigin(const Point &neighboringPoint, const Point &originPoint);
};

#endif
