#ifndef POINT_HXX_
#define POINT_HXX_

#include "Settings.hxx"
#include <cmath>

class Point
{
public:
  constexpr Point() : x(0), y(0), z(0), height(0){};
  constexpr Point(int x, int y) : x(x), y(y), z(0), height(0){};
  constexpr Point(int x, int y, int z) : x(x), y(y), z(z), height(0){};
  constexpr Point(int x, int y, int z, int height) : x(x), y(y), z(z), height(height){};

  /// The x coordinate.
  int x;

  /// The y coordinate.
  int y;

  /// The z coordinate.
  int z;

  /// The height level.
  int height;

  //TODO: need to add z axis in the future, currently it is not supported.
  bool operator==(const Point &p) const { return x == p.x && y == p.y /*&& z == p.z*/; }
  bool operator!=(const Point &p) const { return !(*this == p); }

  static constexpr Point INVALID() { return {-1, -1, -1, -1}; }

  bool isWithinMapBoundaries() const
  {
    return (x >= 0 && x < Settings::instance().mapSize) && (y >= 0 && y < Settings::instance().mapSize);
  }

  /**
   * @brief Checks if a given points is a neighbor of this point
   *
   * @param coordinateOne Point to check if it's adjacent
   * @return if the two points are neighbors
   */
  bool isNeighborOf(Point cooridnate) const
  {
    for (int xOffset = -1; xOffset <= 1; ++xOffset)
    {
      for (int yOffset = -1; yOffset <= 1; ++yOffset)
      {
        Point neighbor;
        Point thisPoint = {x, y};
        neighbor.x = cooridnate.x + xOffset;
        neighbor.y = cooridnate.y + yOffset;
        if (neighbor.isWithinMapBoundaries() && neighbor == thisPoint)
        {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * @brief Check if a given point is a direct neighbor (cardinal direction) of this point
   *
   * @param coordinate - Point to check if it is a direct neighbor
   * @return if point is a direct neighbor in a cardinal direction (top, bottom, left, right)
   */
  bool isDirectNeighborOf(Point coordinate) const{
        Point thisPoint = {x, y};

        if ((thisPoint.x == coordinate.x) && (std::abs(thisPoint.y - coordinate.y) <= 1) ||
        (thisPoint.y == coordinate.y) && (std::abs(thisPoint.x - coordinate.x) <= 1))
    {
      return true;
    }

    return false;
  }

  /**
   * @brief Calculate the manhattan distance between this point and a given point
   *
   * @param target the point to calculate the manhattan distance to
   * @return number of nodes between this point and the target
   */
  int manhattanDistanceTo(Point target) const { return abs(x - target.x) + abs(y - target.y); }

  /**
   * @brief Calculate the direct distance between this point and a given point
   *
   * @param target the point to calculate the direct distance to
   * @return number of nodes between this point and the target
   */
  int distanceTo(Point target) const { return sqrt((x - target.x) * (x - target.x) + (y - target.y) * (y - target.y)); }
};

#endif
