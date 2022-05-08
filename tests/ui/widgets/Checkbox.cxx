#include <catch.hpp>
#include "../../src/engine/ui/widgets/Checkbox.hxx"

TEST_CASE("I can create a Checkbox widget", "[ui][widgets][checkbox]")
{
  std::unique_ptr<Checkbox> cb = std::make_unique<Checkbox>(SDL_Rect{0, 0, 0, 0});
  CHECK(cb != nullptr);
  CHECK(cb->getUiElementData().text.empty());
}

TEST_CASE("I can set the check state of a Checkbox widget", "[ui][widgets][checkbox]")
{
  std::unique_ptr<Checkbox> cb = std::make_unique<Checkbox>(SDL_Rect{0, 0, 0, 0});
  cb->setCheckState(true);
  CHECK(cb->getCheckState());
  CHECK(cb->getButtonState() == BUTTONSTATE_CLICKED);
  cb->setCheckState(false);
  CHECK(!cb->getCheckState());
  CHECK(cb->getButtonState() == BUTTONSTATE_DEFAULT);
}

TEST_CASE("I can reposition a Checkbox widget", "[ui][widgets][checkbox]")
{
  std::unique_ptr<Checkbox> cb = std::make_unique<Checkbox>(SDL_Rect{0, 0, 0, 0});
  cb->setPosition(5, 5);
  CHECK(cb->getUiElementRect().x == 5);
  CHECK(cb->getUiElementRect().y == 5);
}