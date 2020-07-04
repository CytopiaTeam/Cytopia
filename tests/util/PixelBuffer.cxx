#include <catch2/catch.hpp>
#include <array>
#include <vector>
#include "PixelBuffer.hxx"

TEST_CASE("I can create PixelBuffers", "[util]")
{
  const std::vector<uint32_t> noPixels;
  const std::array<uint32_t, 1> oneBlank{0x0};
  PixelBuffer pb1{Rectangle::EMPTY()};
  PixelBuffer pb2{Rectangle{1, 1, 10, 10}};
  PixelBuffer pb3{Rectangle::EMPTY(), noPixels};
  PixelBuffer pb4{Rectangle{0, 0, 0, 0}, oneBlank};
  REQUIRE_THROWS_AS(PixelBuffer(Rectangle{1, 1, 10, 10}, noPixels), CytopiaError);
  REQUIRE_THROWS_AS(PixelBuffer(Rectangle::EMPTY(), oneBlank), CytopiaError);
  REQUIRE(pb1.pixels() == Range{noPixels});
  REQUIRE(pb1.bounds().isEmpty());
  REQUIRE(pb1.isEmpty());
  REQUIRE(pb3.pixels() == Range{noPixels});
  REQUIRE(pb3.bounds().isEmpty());
  REQUIRE(pb3.isEmpty());

  std::vector<uint32_t> hundredEmpty(100, 0x0);
  REQUIRE(pb2.pixels() == Range{hundredEmpty});
  REQUIRE(pb2.bounds() == Rectangle{1, 1, 10, 10});
  REQUIRE(!pb2.isEmpty());
  REQUIRE(pb4.pixels() == Range{oneBlank});
  REQUIRE(pb4.bounds() == Rectangle{0, 0, 0, 0});
  REQUIRE(!pb4.isEmpty());
}

TEST_CASE("I can scale PixelBuffers", "[util]")
{
  /* Basic test on EMPTY */
  PixelBuffer pb1 = PixelBuffer::EMPTY();
  pb1.scale(100);
  REQUIRE(pb1.isEmpty());
  pb1.scale(0.5f);
  REQUIRE(pb1.isEmpty());

  /* Multiple complex tests */
  const std::array initial_bounds{Rectangle{0, 0, 0, 0}, Rectangle{1, 1, 2, 2}, Rectangle{0, 0, 1, 1}, Rectangle{3, 3, 4, 4}};
  const std::array factors{10.f, 2.f, 2.f, 2.f};
  const std::array target_bounds{Rectangle{0, 0, 9, 9}, Rectangle{1, 1, 4, 4}, Rectangle{0, 0, 3, 3}, Rectangle{3, 3, 6, 6}};
  const std::array initial_pixels{std::vector<uint32_t>{0xFFFFFFFF},
                                  std::vector<uint32_t>{0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFFFFFFFF},
                                  std::vector<uint32_t>{0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFFFFFFFF},
                                  std::vector<uint32_t>{0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFFFFFFFF}};
  const std::vector<uint32_t> fourScaled{
      0xFF0000FF, 0xFF0000FF, 0x00FF00FF, 0x00FF00FF,

      0xFF0000FF, 0xFF0000FF, 0x00FF00FF, 0x00FF00FF,

      0x0000FFFF, 0x0000FFFF, 0xFFFFFFFF, 0xFFFFFFFF,

      0x0000FFFF, 0x0000FFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  };
  const std::vector hundredWhites(100, 0xFFFFFFFF);
  const std::array target_pixels{hundredWhites, fourScaled, fourScaled, fourScaled};

  for (int i = 0; i < initial_bounds.size(); i++)
  {
    LOG(LOG_DEBUG) << "PixelBuffer Test " << i;
    PixelBuffer pb{initial_bounds[i], initial_pixels[i]};
    REQUIRE(pb.bounds() == initial_bounds[i]);
    REQUIRE(pb.pixels() == Range{initial_pixels[i]});
    pb.scale(factors[i]);
    REQUIRE(pb.bounds() == target_bounds[i]);
    REQUIRE(pb.pixels() == Range{target_pixels[i]});
    pb.scale(1.f / factors[i]);
    REQUIRE(pb.bounds() == initial_bounds[i]);
    REQUIRE(pb.pixels() == Range{initial_pixels[i]});
  }
}

TEST_CASE("I can recolor PixelBuffers", "[util]")
{
  /* These colors are not affected by 
   * magic pixel recoloring because
   * They are either not magic pixels,
   * or they hit the extremes of overlay
   * blending
   */
  const std::array unaffected_colors{
      0x000000FF_rgba, /* Pure black: Always overlay back to black */
      0xFFFFFFFF_rgba, /* Pure white: Always overlay back to white */
      0xFF0000FF_rgba, /* Pure red */
      0x0000FFFF_rgba  /* Pure blue */
      /* ... and more. There are tons of non-magic colors */
  };

  Rectangle r1{1, 0, unaffected_colors.size(), 0};
  PixelBuffer pb1{r1, unaffected_colors};
  pb1.colorMagicPixels(0x00000000_rgba);
  REQUIRE(pb1.pixels() == Range{unaffected_colors});
  pb1.colorMagicPixels(0xFF000000_rgba);
  REQUIRE(pb1.pixels() == Range{unaffected_colors});
  pb1.colorMagicPixels(0x00FF00000_rgba);
  REQUIRE(pb1.pixels() == Range{unaffected_colors});
  pb1.colorMagicPixels(0x0000FF00_rgba);
  REQUIRE(pb1.pixels() == Range{unaffected_colors});
  pb1.colorMagicPixels(0xFFFFFF00_rgba);
  REQUIRE(pb1.pixels() == Range{unaffected_colors});

  /* Before we test other colors, take notes of the following facts
   * lightness() == 0 implies color == black.
   * lightness() == 255 implies color == white
   * r = g = b !in {0, 255} implies color is a grayscale
   * r = b < g implies hue() == 210   (green)
   * r = b > g implies hue() == 84    (purple)
   *
   * Hence, there are three situations we are interested in testing:
   */

  /** grayscales: 
    * Pick a random digit X in [1, 254]
    * our Magic pixel is RGB { X, X, X }
    * Pick a random digit Y in [0, 255]
    * our Target pixel is RGB { Y, Y, Y }
    * Expected result is
    * Z = {
    *     if X < 128
    *     2 * X * Y / 255
    *     otherwise
    *     255 - 2 * (255 - X) * (255 - Y) / 255
    *    }
    * There are 254 * 256 = 65,024 possibilities
    */
  const std::array grayscales_colors{0x101010FF_rgba, 0x303030FF_rgba, 0xA0A0A0FF_rgba, 0xD0D0D0FF_rgba, 0xF0F0F0FF_rgba};
  const std::array<uint32_t, 5> expected_grayscales{0x171717FF_rgba, 0x464646FF_rgba, 0xCDCDCDFF_rgba, 0xE6E6E6FF_rgba,
                                                    0xF7F7F7FF_rgba};
  Rectangle r2{1, 0, grayscales_colors.size(), 0};
  PixelBuffer pb2{r2, grayscales_colors};
  pb2.colorMagicPixels(0xBBBBBBFF_rgba);
  REQUIRE(pb2.pixels() == Range{expected_grayscales});

  /** purples: 
    * Pick random digit X in [1, 253]
    * Pick random digit Y in [X + 1, 254]
    * our Magic pixel is RGB { Y, X, Y }
    * Pick a random target pixel in [0, 255]^3
    */
  const std::array purples_colors{0xD030D0FF_rgba, 0x313031FF_rgba, 0x310031FF_rgba, 0xFF00FFFF_rgba, 0x010001FF_rgba};
  const std::array<uint32_t, 5> expected_purples{0x2ae8e8ff, 0x333333ff, 0x3232ff, 0xfffffff, 0xff};
  Rectangle r3{1, 0, purples_colors.size(), 0};
  PixelBuffer pb3{r3, purples_colors};
  pb3.colorMagicPixels(0x30A0E0FF_rgba);
  REQUIRE(pb3.pixels() == Range{expected_purples});

  /*
   * Saturation before: 160 -> 62
   * Saturation after:  206 -> 80 OK
   * 
   * Saturation Target: 188 -> 74
   * Lightness Target:  135 -> 53
   *
   * Lightness before:  128 -> 50
   * Lightness after:   137 -> 53 OK
   *
   */

  /** greens: 
    * Pick random digit X in [1, 253]
    * Pick random digit Y in [X + 1, 254]
    * our Magic pixel is RGB { Y, X, Y }
    * Pick a random target pixel in [0, 255]^3
    */
  const std::array greens_colors{0x00FF00FF_rgba, 0xFEFFFEFF_rgba, 0x000100FF_rgba, 0xFFA0FFFF_rgba, 0xB000B0FF_rgba};
  const std::array expected_greens{0xdc6eff_rgba, 0xfdfffeff_rgba, 0xFF_rgba, 0x93ffc9ff_rgba, 0x984cff_rgba};
  Rectangle r4{1, 0, greens_colors.size(), 0};
  PixelBuffer pb4{r4, greens_colors};
  pb4.colorMagicPixels(0xDEC06D_rgba);
  REQUIRE(pb4.pixels() == Range{expected_greens});
}
