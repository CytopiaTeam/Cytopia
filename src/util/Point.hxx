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

bool operator==(const Point2D &, const Point2D &);

template<> struct std::hash<Point2D>
{
  std::size_t operator()(const Point2D & p) const noexcept;
};

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
