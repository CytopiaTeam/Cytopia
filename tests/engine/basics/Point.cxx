#include <catch.hpp>
#include "../../../src/engine/basics/Point.hxx"


TEST_CASE("I can construct points", "[point]")
{
  Point p0;
  CHECK(p0.x == 0);
  CHECK(p0.y == 0);
  CHECK(p0.z == 0);
  CHECK(p0.height == 0);

  Point p1(1, 2);
  CHECK(p1.x == 1);
  CHECK(p1.y == 2);
  CHECK(p1.z == 0);
  CHECK(p1.height == 0);

  Point p2(1, 2, 3);
  CHECK(p2.x == 1);
  CHECK(p2.y == 2);
  CHECK(p2.z == 3);
  CHECK(p2.height == 0);

  Point p3(1, 2, 3, 4);
  CHECK(p3.x == 1);
  CHECK(p3.y == 2);
  CHECK(p3.z == 3);
  CHECK(p3.height == 4);
}

TEST_CASE("I can compare points for equality", "[point]")
{
  Point p1(1, 2);
  Point p2(1, 2, 3);

  CHECK(p1 == p2);
  CHECK(!(p1 != p2));

  CHECK(p1 == p1);
  CHECK(!(p1 != p1));

  Point p3;
  CHECK(p1 != p3);
  CHECK(!(p1 == p3));

  CHECK(Point::INVALID() == Point::INVALID());
}

TEST_CASE("Point::isWithinMapBoundaries", "[point]")
{
  Settings::instance().mapSize = 10;

  CHECK(Point().isWithinMapBoundaries());
  CHECK(!Point::INVALID().isWithinMapBoundaries());

  CHECK(Point(1, 2).isWithinMapBoundaries());
  CHECK(Point(1, 2, 30, -10).isWithinMapBoundaries());

  CHECK(!Point(10, 1).isWithinMapBoundaries());
  CHECK(!Point(1, 10).isWithinMapBoundaries());

  CHECK(!Point(-1, 1).isWithinMapBoundaries());
  CHECK(!Point(1, -1).isWithinMapBoundaries());
}

TEST_CASE("Point::isNeighbourOf", "[point]")
{
  Point p(1, 2);
  for (int xoff = -1; xoff <= 1; ++xoff) {
    for (int yoff = -1; yoff <= 1; ++yoff) {
      CHECK(p.isNeighborOf(Point(p.x + xoff, p.y + yoff)));
    }
  }

  CHECK(!Point(0, 1).isNeighborOf(Point(2, 1)));
  CHECK(!Point(3, 7).isNeighborOf(Point(3, 5)));

  CHECK(p.isNeighborOf(Point(p.x, p.y, 3, 4)));

  // FIXME Not symmetric?
  CHECK(!Point(-1, 1).isNeighborOf(Point( 0, 1)));
  CHECK( Point( 0, 1).isNeighborOf(Point(-1, 1)));
}


TEST_CASE("Point::isDirectNeighborOf", "[point]")
{
  Point p(1, 2);

  CHECK(p.isDirectNeighborOf(Point(0, 2)));
  CHECK(p.isDirectNeighborOf(Point(1, 2)));
  CHECK(p.isDirectNeighborOf(Point(2, 2)));
  CHECK(p.isDirectNeighborOf(Point(1, 1)));
  CHECK(p.isDirectNeighborOf(Point(1, 3)));

  CHECK(!p.isDirectNeighborOf(Point(0, 1)));
  CHECK(!p.isDirectNeighborOf(Point(0, 3)));
  CHECK(!p.isDirectNeighborOf(Point(2, 3)));
  CHECK(!p.isDirectNeighborOf(Point(2, 1)));

  CHECK(!p.isDirectNeighborOf(Point(1, 10)));
  CHECK(!p.isDirectNeighborOf(Point(4, 2)));

  CHECK(p.isDirectNeighborOf(Point(p.x, p.y, 3, 4)));
}


TEST_CASE("Point::manhattanDistanceTo", "[point]")
{
  CHECK(Point(1, 2).manhattanDistanceTo(Point(1, 2)) == 0);
  CHECK(Point(1, 2).manhattanDistanceTo(Point(0, 2)) == 1);
  CHECK(Point(1, 2).manhattanDistanceTo(Point(2, 2)) == 1);
  CHECK(Point(1, 2).manhattanDistanceTo(Point(2, 3)) == 2);
  CHECK(Point(2, 1).manhattanDistanceTo(Point(6,-1)) == 6);
}
