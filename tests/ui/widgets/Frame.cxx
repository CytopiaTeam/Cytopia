#include <catch.hpp>
#include "../../src/engine/ui/widgets/Frame.hxx"
#include "../../Common.hxx"

TEST_CASE("I can create a Frame widget", "[ui][widgets][frame]")
{
  START_TEST

  std::unique_ptr<Frame> frame = std::make_unique<Frame>(SDL_Rect{0, 0, 0, 0});
  CHECK(frame != nullptr);

  FINISH_TEST
}

TEST_CASE("I can draw a Frame widget", "[ui][widgets][frame]")
{
  START_TEST

  std::unique_ptr<Frame> frame = std::make_unique<Frame>(SDL_Rect{0, 0, 0, 0});
  // TODO: figure out a way to actually check this
  CHECK_NOTHROW(frame->draw());

  FINISH_TEST
}