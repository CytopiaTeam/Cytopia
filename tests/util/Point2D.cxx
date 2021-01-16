#include "Point2D.hxx"
#include "MouseEvents.hxx"
#include "LOG.hxx"
#include <catch.hpp>

TEST_CASE("I can use 2D Points", "[util]")
{
  Point2D p1{0, 0};
  Point2D p2{0, 0};
  Point2D p3{0, 1};
  REQUIRE(p1 == p2);
  REQUIRE(p2 == p1);
  REQUIRE(p1 != p3);
  REQUIRE(p3 != p1);
  REQUIRE(p2 != p3);
  REQUIRE(p3 != p2);
  std::hash<Point2D> h;
  REQUIRE(h(p1) == h(p2));
  REQUIRE(h(p1) != h(p3));
  REQUIRE_NOTHROW(LOG(LOG_INFO) << p1);
}

TEST_CASE("I can use Spatial Blocks", "[util]")
{
  constexpr auto N = SpatialBlock::BlockSize::value;
  SpatialBlock sb1{1, 1};
  SpatialBlock sb2{1, 1};
  SpatialBlock sb3{2, 2};
  REQUIRE(sb1 == sb2);
  REQUIRE(sb2 == sb1);
  REQUIRE(sb1 != sb3);
  REQUIRE(sb3 != sb1);
  REQUIRE(sb2 != sb3);
  REQUIRE(sb3 != sb2);
  std::hash<SpatialBlock> h;
  REQUIRE(h(sb1) == h(sb2));
  REQUIRE(h(sb1) != h(sb3));
  REQUIRE(sb1 == SpatialBlock{ClickEvent{N * 3 / 2, N * 3 / 2}});
  REQUIRE(sb1 == SpatialBlock{MousePositionEvent{N * 3 / 2, N * 3 / 2, 0, 0}});
  REQUIRE_NOTHROW(LOG(LOG_INFO) << sb1);
}
