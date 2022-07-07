#include <catch.hpp>
#include "../../src/engine/ui/widgets/Image.hxx"
#include "../../Common.hxx"

TEST_CASE("I can create an Image widget", "[ui][widgets][image]")
{
  START_TEST

  std::unique_ptr<Image> image = std::make_unique<Image>();
  CHECK(image != nullptr);

  FINISH_TEST
}

TEST_CASE("I can draw an Image widget", "[ui][widgets][image]")
{
  START_TEST

  std::unique_ptr<Image> image = std::make_unique<Image>();
  // TODO: figure out a way to actually check this
  CHECK_NOTHROW(image->draw());

  FINISH_TEST
}