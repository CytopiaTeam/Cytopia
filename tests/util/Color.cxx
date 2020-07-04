#include "Color.hxx"
#include <catch2/catch.hpp>
#include "LOG.hxx"

TEST_CASE("I can get a Color's hue", "[util]")
{
  REQUIRE((0x0_rgba).hue() == 0);
  REQUIRE((0xFF000000_rgba).hue() == 0);
  REQUIRE((0xFFFF0000_rgba).hue() == 42);
  REQUIRE((0x00FF0000_rgba).hue() == 84);
  REQUIRE((0x00FFFF00_rgba).hue() == 126);
  REQUIRE((0x0000FF00_rgba).hue() == 168);
  REQUIRE((0xFF00FF00_rgba).hue() == 210);
  REQUIRE((0xFFFFFF00_rgba).hue() == 0);
  REQUIRE((0xFF00FF00_rgba).hue() == 210);
  REQUIRE(Palette::Orange.hue() == 16);
}

TEST_CASE("I can get a Color's lightness", "[util]")
{
  REQUIRE((0x0_rgba).lightness() == 0);
  REQUIRE((0xFF000000_rgba).lightness() == 127);
  REQUIRE((0xFFFF0000_rgba).lightness() == 127);
  REQUIRE((0xFF0000_rgba).lightness() == 127);
  REQUIRE((0xFFFF00_rgba).lightness() == 127);
  REQUIRE((0xFF00_rgba).lightness() == 127);
  REQUIRE((0xFF00FF00_rgba).lightness() == 127);
  REQUIRE((0xFFFFFF00_rgba).lightness() == 255);
  REQUIRE((0xC0C0C000_rgba).lightness() == 192);
  REQUIRE((0x80808000_rgba).lightness() == 128);
  REQUIRE((0x40404000_rgba).lightness() == 64);
  REQUIRE((0xFF808000_rgba).lightness() == 191);
  REQUIRE((0x80FF8000_rgba).lightness() == 191);
  REQUIRE((0x8080FF00_rgba).lightness() == 191);
  REQUIRE(Palette::Orange.lightness() == 142);
}

TEST_CASE("I can get a Color's saturation", "[util]")
{
  REQUIRE((0x0_rgba).saturation() == 0);
  REQUIRE((0xFFFFFF00_rgba).saturation() == 0);
  REQUIRE((0xC0C0C000_rgba).saturation() == 0);
  REQUIRE((0x80808000_rgba).saturation() == 0);
  REQUIRE((0x40404000_rgba).saturation() == 0);
  REQUIRE((0x6285A100_rgba).saturation() == 64);
  REQUIRE((0x4187C000_rgba).saturation() == 128);
  REQUIRE((0x2190E000_rgba).saturation() == 192);
  REQUIRE((0x008CFF00_rgba).saturation() == 255);
  REQUIRE(Palette::Orange.saturation() == 255);
}

TEST_CASE("I can retrieve RGBA values from a Color", "[util]")
{
  REQUIRE((0x01020304_rgba).red() == 0x01);
  REQUIRE((0x01020304_rgba).green() == 0x02);
  REQUIRE((0x01020304_rgba).blue() == 0x03);
  REQUIRE((0x01020304_rgba).alpha() == 0x04);
  LOG(LOG_INFO) << Palette::Orange;
  SDL_Color color = (0x01020304_rgba).to_SDL();
  REQUIRE(color.r == 0x1);
  REQUIRE(color.g == 0x2);
  REQUIRE(color.b == 0x3);
  REQUIRE(color.a == 0x4);
}
