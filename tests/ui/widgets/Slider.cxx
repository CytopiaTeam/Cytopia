#include <catch.hpp>
#include "../../src/engine/ui/widgets/Slider.hxx"

TEST_CASE("I can create a Slider widget", "[ui][widgets][slider]")
{
  std::unique_ptr<Slider> slider = std::make_unique<Slider>(SDL_Rect{0, 0, 0, 0});
  CHECK(slider != nullptr);
  CHECK(slider->getUiElementData().text.empty());
}

TEST_CASE("I can draw a Slider widget", "[ui][widgets][slider]")
{
  std::unique_ptr<Slider> slider = std::make_unique<Slider>(SDL_Rect{0, 0, 0, 0});
  //@todo actually verify it draws the slider
  CHECK_NOTHROW(slider->draw());
}

TEST_CASE("Setting a value outside the range does not change the value of a Slider widget", "[ui][widgets][slider]")
{
  std::unique_ptr<Slider> slider = std::make_unique<Slider>(SDL_Rect{0, 0, 0, 0});
  int oldVal = slider->getValue();
  slider->setValue(-5);
  CHECK(slider->getValue() == oldVal);
  oldVal = slider->getValue(); // just in case the previous check fails it won't affect the next one
  slider->setValue(500);
  CHECK(slider->getValue() == oldVal);
}

TEST_CASE("I can set the value of a Slider widget", "[ui][widgets][slider]") 
{
  std::unique_ptr<Slider> slider = std::make_unique<Slider>(SDL_Rect{0, 0, 0, 0});
  slider->setValue(50);
  CHECK(slider->getValue() == 50);
  // TODO: check that the slider button is in the correct place. somehow.
}

TEST_CASE("I can reposition a Slider widget", "[ui][widgets][slider]")
{
  std::unique_ptr<Slider> slider = std::make_unique<Slider>(SDL_Rect{0, 0, 0, 0});
  slider->setPosition(5, 5);
  CHECK(slider->getUiElementRect().x == 5);
  CHECK(slider->getUiElementRect().y == 5);
  // TODO: Check that the slider button is also repositioned correctly. Somehow.
}