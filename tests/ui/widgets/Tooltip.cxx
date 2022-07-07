#include <catch.hpp>
#include <SDL_ttf.h>
#include "../../src/engine/ui/widgets/Tooltip.hxx"
#include "../../Common.hxx"

TEST_CASE("I can create a Tooltip widget", "[ui][widgets][tooltip]")
{
  START_TEST

  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  CHECK(tip != nullptr);
  CHECK(tip->getUiElementData().text.empty());
  CHECK(!tip->isVisible());

  FINISH_TEST
}

TEST_CASE("I can draw a Tooltip widget", "[ui][widgets][tooltip]")
{
  START_TEST

  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  CHECK_NOTHROW(tip->draw());
  tip->showTooltip();
  CHECK_NOTHROW(tip->draw());

  FINISH_TEST
}

TEST_CASE("I can set text on a Tooltip widget", "[ui][widgets][tooltip]")
{
  START_TEST

  TTF_Init();
  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  tip->setText("Test");
  CHECK(tip->getUiElementData().text == "Test");

  FINISH_TEST
}

TEST_CASE("I can show a Tooltip widget and it will be visible", "[ui][widgets][tooltip]") 
{
  START_TEST

  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  tip->showTooltip();
  CHECK(tip->isVisible());

  FINISH_TEST
}

TEST_CASE("I can reset a Tooltip widget and it will be hidden", "[ui][widgets][tooltip]") 
{
  START_TEST

  std::unique_ptr<Tooltip> tip = std::make_unique<Tooltip>();
  tip->startTimer();
  tip->reset();
  CHECK(!tip->isVisible());

  FINISH_TEST
}