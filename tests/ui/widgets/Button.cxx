#include <catch.hpp>
#include <SDL_ttf.h>
#include "../../src/engine/ui/widgets/Button.hxx"
#include "../../src/engine/ui/widgets/Text.hxx"
#include "../../src/engine/basics/Settings.hxx"


TEST_CASE("I can create a Button widget", "[ui][widgets][button]")
{
  std::unique_ptr<Button> button = std::make_unique<Button>(SDL_Rect{0, 0, 0, 0});
  CHECK(button != nullptr);
  CHECK(button->getUiElementData().text.empty());
}

TEST_CASE("I can set the check state of a Button widget", "[ui][widgets][button]")
{
  std::unique_ptr<Button> button = std::make_unique<Button>(SDL_Rect{0, 0, 0, 0});
  button->setCheckState(true);
  CHECK(button->checkState());
  CHECK(button->getButtonState() == BUTTONSTATE_CLICKED);
  button->setCheckState(false);
  CHECK(!button->checkState());
  CHECK(button->getButtonState() == BUTTONSTATE_DEFAULT);
}

TEST_CASE("I can set text of a Button widget", "[ui][widgets][button]")
{
  TTF_Init();
  std::unique_ptr<Button> button = std::make_unique<Button>(SDL_Rect{0, 0, 0, 0});
  button->setText("Test");
  CHECK(button->getUiElementData().text == "Test");
}