#ifndef POINT_HXX_
#define POINT_HXX_

class Point
{
public:
  /**
   * The x coordinate.
   */
  int x;

  /**
   * The y coordinate.
   */
  int y;

  /**
   * The z coordinate.
   */
  int z;

  /**
   * The height level.
   */
  int height;

  //TODO: need to add z axis in the future, currently it is not supported.
  bool operator==(const Point &p) const { return x == p.x && y == p.y /*&& z == p.z*/; }
  bool operator!=(const Point &p) const { return !(*this == p); }

  static constexpr Point INVALID() { return {-1, -1, -1, -1}; }
};


#endif
