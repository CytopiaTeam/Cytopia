#ifndef POINT_HXX_
#define POINT_HXX_

#include "Settings.hxx"
#include <cmath>

class Point
{
public:
  constexpr Point() : x(0), y(0), z(0), height(0), rawHeight(0){};
  constexpr Point(int x, int y) : x(x), y(y), z(0), height(0), rawHeight(0){};
  constexpr Point(int x, int y, int z) : x(x), y(y), z(z), height(0), rawHeight(0){};
  constexpr Point(int x, int y, int z, int height) : x(x), y(y), z(z), height(height), rawHeight(0){};
  constexpr Point(int x, int y, int z, int height, double rawHeight) : x(x), y(y), z(z), height(height), rawHeight(rawHeight){};

  /// The x coordinate.
  int x;

  /// The y coordinate.
  int y;

  /// The z coordinate.
  int z;

  /// The height level.
  int height;

  /// The raw height.
  double rawHeight;

  //TODO: need to add z axis in the future, currently it is not supported.
  bool operator==(const Point &p) const { return x == p.x && y == p.y /*&& z == p.z*/; }
  bool operator!=(const Point &p) const { return !(*this == p); }

  static constexpr Point INVALID() { return {-1, -1, -1, -1}; }

  bool isWithinMapBoundaries() const
  {
    return (x >= 0 && x < Settings::instance().mapSize) && (y >= 0 && y < Settings::instance().mapSize);
  }

  /**
   * @brief Checks if a given point is a neighbor of this point
   * @param coordinate Point to check if it's adjacent
   * @return if the two points are neighbors
   */
  bool isNeighborOf(Point coordinate) const
  {
    return isWithinMapBoundaries() && std::max(std::abs(coordinate.x - x), std::abs(coordinate.y - y)) <= 1;
  }

  /**
   * @brief Check if a given point is a direct neighbor (cardinal direction) of this point
   * @param coordinate Point to check if it is a direct neighbor
   * @return if point is a direct neighbor in a cardinal direction (top, bottom, left, right)
   */
  bool isDirectNeighborOf(Point coordinate) const
  {
    return ((x == coordinate.x) && (std::abs(y - coordinate.y) <= 1)) ||
           ((y == coordinate.y) && (std::abs(x - coordinate.x) <= 1));
  }

  /**
   * @brief Calculate the manhattan distance between this point and a given point
   * @param target the point to calculate the manhattan distance to
   * @return number of nodes between this point and the target
   */
  int manhattanDistanceTo(Point target) const { return abs(x - target.x) + abs(y - target.y); }

  /**
   * @brief Calculate the direct distance between this point and a given point
   * @param target the point to calculate the direct distance to
   * @return number of nodes between this point and the target
   */
  int distanceTo(Point target) const { return sqrt((x - target.x) * (x - target.x) + (y - target.y) * (y - target.y)); }

  /**
  * @brief calculates the index (stride) that can be used to access in Map to access mapNodes vector. 
  */
  int toIndex() const { return x * Settings::instance().mapSize + y; };
};

namespace std
{
template <> struct hash<Point>
{
  const size_t operator()(const Point &p) const { return std::hash<int>()(p.x) ^ std::hash<int>()(p.y); }
};
} // namespace std

#endif
