#include <catch.hpp>
#include "../../src/engine/ui/widgets/Slider.hxx"

TEST_CASE("I can create a Slider widget", "[ui][widgets][slider]")
{
  std::unique_ptr<Slider> slider = std::make_unique<Slider>(SDL_Rect{0, 0, 0, 0});
  CHECK(slider != nullptr);
  CHECK(slider->getUiElementData().text.empty());
}

// check it can set values