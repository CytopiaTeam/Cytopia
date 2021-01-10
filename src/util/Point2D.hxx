#ifndef POINT_2D_HXX_
#define POINT_2D_HXX_

#include "Meta.hxx"
#include <iostream>
#include "../events/MouseEvents.hxx"

/**
 * @brief   A 2D point
 */
struct Point2D
{
  int x;
  int y;
};

bool operator==(const Point2D &, const Point2D &);
bool operator!=(const Point2D &, const Point2D &);
std::ostream &operator<<(std::ostream &, const Point2D &);

template <> struct std::hash<Point2D>
{
  std::size_t operator()(const Point2D &p) const noexcept;
};

/**
 * @brief   A block of space
 * @details A SpatialBlock represents a Rectangle {x, y, x + BlockSize, y + BlockSize}
 */
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
bool operator!=(const SpatialBlock &, const SpatialBlock &);
std::ostream &operator<<(std::ostream &, const SpatialBlock &);

template <> struct std::hash<SpatialBlock>
{
  std::size_t operator()(const SpatialBlock &b) const noexcept;
};

#endif // POINT_2D_HXX_
