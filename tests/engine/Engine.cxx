#include <catch.hpp>
#include "../../src/engine/Engine.hxx"

using string = std::string;

TEST_CASE("Get Engine instance", "[engine][engine]")
{
  void *singleton1 = static_cast<void *>(&Engine::instance());
  void *singleton2 = static_cast<void *>(&Engine::instance());
  CHECK(singleton1 == singleton2);
}

//TEST_CASE("Start Engine", "[engine][engine]")
//{
//  Engine &engine = Engine::instance();
//  engine.newGame();
//  engine.quitGame();
//  CHECK(engine.isGameRunning() == false);
//}
