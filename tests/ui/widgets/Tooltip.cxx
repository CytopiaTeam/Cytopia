#include <catch.hpp>
#include <SDL_ttf.h>
#include "../../src/engine/ui/widgets/Tooltip.hxx"

TEST_CASE("I can create a Tooltip widget", "[ui][widgets][tooltip]")
{
  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  CHECK(tip != nullptr);
  CHECK(tip->getUiElementData().text.empty());
  CHECK(!tip->isVisible());
}

TEST_CASE("I can draw a Tooltip widget", "[ui][widgets][tooltip]")
{
  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  CHECK_NOTHROW(tip->draw());
  tip->showTooltip();
  CHECK_NOTHROW(tip->draw());
}

TEST_CASE("I can set text on a Tooltip widget", "[ui][widgets][tooltip]")
{
  TTF_Init();
  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  tip->setText("Test");
  CHECK(tip->getUiElementData().text == "Test");
}

TEST_CASE("I can show a Tooltip widget and it will be visible", "[ui][widgets][tooltip]") 
{
  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  tip->showTooltip();
  CHECK(tip->isVisible());
}

TEST_CASE("I can reset a Tooltip widget and it will be hidden", "[ui][widgets][tooltip]") 
{
  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  tip->startTimer();
  tip->reset();
  CHECK(!tip->isVisible());
}