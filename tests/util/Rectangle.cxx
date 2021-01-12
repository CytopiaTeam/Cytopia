#include "Rectangle.hxx"
#include <catch.hpp>
#include "Point2D.hxx"
#include "LOG.hxx"
#include <SDL2/SDL.h>

TEST_CASE("I can create Rectangles", "[util]")
{
  Rectangle r1{0, 0, 9, 9};
  Rectangle r2{9, 0, 0, 9};
  Rectangle r3{9, 9, 0, 0};
  Rectangle r4{0, 9, 9, 0};
  REQUIRE(r1 == r2);
  REQUIRE(r2 == r1);
  REQUIRE(r2 == r3);
  REQUIRE(r3 == r2);
  REQUIRE(r3 == r4);
  REQUIRE(r4 == r3);
  REQUIRE(r4 == r1);
  REQUIRE(r1 == r4);
  REQUIRE(Rectangle::EMPTY() == Rectangle::EMPTY());
  REQUIRE_NOTHROW(LOG(LOG_INFO) << r1);
  REQUIRE_NOTHROW(LOG(LOG_INFO) << Rectangle::EMPTY());
  SDL_Rect sdl_rect;
  REQUIRE_NOTHROW(sdl_rect = r1.to_SDL());
  REQUIRE(sdl_rect.x == r1.x1());
  REQUIRE(sdl_rect.y == r1.y1());
  REQUIRE(sdl_rect.w == r1.width());
  REQUIRE(sdl_rect.h == r1.height());
}

TEST_CASE("I can test if Rectangles contain a Point", "[util]")
{
  Rectangle r{0, 0, 0, 0};
  REQUIRE(r.contains(Point2D{0, 0}));
  r = {0, 0, 1, 0};
  REQUIRE(r.contains(Point2D{1, 0}));
  r = {0, 0, 0, 1};
  REQUIRE(r.contains(Point2D{0, 1}));
  r = {1, 1, 1, 1};
  REQUIRE(!r.contains(Point2D{0, 1}));
  REQUIRE(!r.contains(Point2D{1, 0}));
  REQUIRE(!r.contains(Point2D{0, 0}));
  REQUIRE(!r.contains(Point2D{0, 2}));
  REQUIRE(!r.contains(Point2D{2, 0}));
  REQUIRE(!r.contains(Point2D{1, 2}));
  REQUIRE(!r.contains(Point2D{2, 1}));
  REQUIRE(!r.contains(Point2D{2, 2}));
  REQUIRE(r.contains(Point2D{1, 1}));
}

TEST_CASE("I can get the bounds of a Rectangle", "[util]")
{
  Rectangle x{0, 0, 0, 0};
  REQUIRE(x == x.getBounds());
  REQUIRE(Rectangle::EMPTY() == Rectangle::EMPTY().getBounds());
}

TEST_CASE("I can get the dimensions of a Rectangle", "[util]")
{
  Rectangle r1{0, 0, 0, 0};
  REQUIRE(r1.width() == 1);
  REQUIRE(r1.height() == 1);
  r1 = {0, 0, 9, 4};
  REQUIRE(r1.width() == 10);
  REQUIRE(r1.height() == 5);
}

TEST_CASE("I can get the points of a Rectangle", "[util]")
{
  Rectangle r{2, 3, 0, 1};
  auto [p1x, p1y] = r.p1();
  auto [p2x, p2y] = r.p2();
  REQUIRE(p1x == 0);
  REQUIRE(p1y == 1);
  REQUIRE(p2x == 2);
  REQUIRE(p2y == 3);

  REQUIRE(r.x1() == 0);
  REQUIRE(r.y1() == 1);
  REQUIRE(r.x2() == 2);
  REQUIRE(r.y2() == 3);
}

TEST_CASE("I can verify if a Rectangle is empty", "[util]")
{
  Rectangle r{0, 0, 0, 0};
  REQUIRE(!r.isEmpty());
  REQUIRE(Rectangle::EMPTY().isEmpty());
}

TEST_CASE("I can intersect two Rectangles", "[util]")
{
  Rectangle r1{1, 1, 10, 10};
  Rectangle r2{0, 0, 0, 0};
  Rectangle r3{0, 0, 11, 11};
  r3.intersect(r1);
  REQUIRE(r3 == r1);
  r2.intersect(r1);
  REQUIRE(r2.isEmpty());
}

TEST_CASE("I can scale a Rectangle around its center", "[util]")
{
  Rectangle r1{5, 5, 5, 5};
  Rectangle r1_target{1, 1, 10, 10};
  int area_before = r1.width() * r1.height();
  r1.scaleCenter(10);
  REQUIRE(r1 == r1_target);
  REQUIRE(area_before * 10 * 10 == r1.width() * r1.height());

  r1 = {5, 5, 5, 5};
  r1.scaleCenter(10.f);
  REQUIRE(r1 == r1_target);
  REQUIRE(area_before * 10 * 10 == r1.width() * r1.height());

  Rectangle r2{1, 1, 10, 10};
  Rectangle r2_target{5, 5, 5, 5};
  r2.scaleCenter(0.1f);
  REQUIRE(r2 == r2_target);

  /* Multiplication by a small or negative empties */
  Rectangle r3{0, 0, 0, 0};
  r3.scaleCenter(std::numeric_limits<float>::denorm_min());
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(std::numeric_limits<double>::denorm_min());
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(std::numeric_limits<long double>::denorm_min());
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0);
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0.f);
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0.l);
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0.L);
  REQUIRE(r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(-100);
  REQUIRE(r3.isEmpty());

  /* Multiplication by a small values divisible by two does not empty */
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0.0001f);
  REQUIRE(!r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0.0001l);
  REQUIRE(!r3.isEmpty());
  r3 = {0, 0, 0, 0};
  r3.scaleCenter(0.0001L);
  REQUIRE(!r3.isEmpty());
}

TEST_CASE("I can translate a Rectangle", "[util]")
{
  Rectangle r{0, 0, 0, 0};
  r.translateX(100);
  REQUIRE(r == Rectangle{100, 0, 100, 0});
  r.translateX(-100);
  REQUIRE(r == Rectangle{0, 0, 0, 0});
  r.translateY(100);
  REQUIRE(r == Rectangle{0, 100, 0, 100});
  r.translateY(-100);
  REQUIRE(r == Rectangle{0, 0, 0, 0});
  r = Rectangle::EMPTY();
  r.translateX(100);
  REQUIRE(r.isEmpty());
  r.translateY(100);
  REQUIRE(r.isEmpty());
}
