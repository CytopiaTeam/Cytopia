#include <catch.hpp>
#include "../../src/engine/ui/widgets/Image.hxx"

TEST_CASE("I can create an Image widget", "[ui][widgets][image]")
{
  std::unique_ptr<Image> image = std::make_unique<Image>();
  CHECK(image != nullptr);
}

TEST_CASE("I can draw an Image widget", "[ui][widgets][image]")
{
  std::unique_ptr<Image> image = std::make_unique<Image>();
  // TODO: figure out a way to actually check this
  CHECK_NOTHROW(image->draw());
}