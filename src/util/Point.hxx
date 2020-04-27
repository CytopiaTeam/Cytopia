#ifndef POINT_HXX_
#define POINT_HXX_

#include "Meta.hxx"

class MousePositionEvent;
class ClickEvent;

struct Point2D
{
  int x;
  int y;
};

struct TilePosition : public Point2D { };

struct ChunkPosition : public Point2D
{
  ChunkPosition();
  explicit ChunkPosition(const TilePosition &);
};

template<> struct std::hash<ChunkPosition>
{
  std::size_t operator()(const Point2D & p) const noexcept
  {
    auto [x, y] = p;
    return x ^ (y << 1);
  }
};

using ChunkSize = Constant<128>;

struct Point
{
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
};

bool operator==(const Point2D &, const Point2D &);

struct SpatialBlock
{
  int x;
  int y;
  explicit SpatialBlock(const MousePositionEvent &);
  explicit SpatialBlock(const ClickEvent &);
  SpatialBlock(int, int);
  using BlockSize = Constant<128>;
};

bool operator==(const SpatialBlock &, const SpatialBlock &);

template <>
struct std::hash<SpatialBlock>
{
  std::size_t operator()(const SpatialBlock & b) const noexcept;
};

#endif
