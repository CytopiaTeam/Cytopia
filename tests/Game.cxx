#include "../src/Game.hxx"
#include <catch2/catch.hpp>

TEST_CASE("I can shutdown a game")
{
  Game game;
  CHECK(game.initialize());
  game.shutdown();
}
