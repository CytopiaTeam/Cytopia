#include "Point.hxx"
#include "../events/MouseEvents.hxx"

ChunkPosition::ChunkPosition() = default;

ChunkPosition::ChunkPosition(const TilePosition & tPos) :
  Point2D { tPos.x / ChunkSize::value, tPos.y / ChunkSize::value }
{ }

SpatialBlock::SpatialBlock(const MousePositionEvent & me) :
  x(me.xPosition / BlockSize::value),
  y(me.yPosition / BlockSize::value)
{
}

SpatialBlock::SpatialBlock(const ClickEvent & ce) :
  x(ce.xPosition / BlockSize::value),
  y(ce.yPosition / BlockSize::value)
{
}

SpatialBlock::SpatialBlock(int x, int y) :
  x(x), y(y) { }

bool operator==(const Point2D & p1, const Point2D & p2)
{
  return p1.x == p2.x && p1.y == p2.y;
}

bool operator==(const SpatialBlock & sb1, const SpatialBlock & sb2)
{
  return sb1.x == sb2.x && sb1.y == sb2.y;
}

std::size_t std::hash<SpatialBlock>::operator()(const SpatialBlock & b) const noexcept
{
  return b.x ^ (b.y << 1);
}
