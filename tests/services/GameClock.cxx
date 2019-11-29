#include <catch2/catch.hpp>
#include <array>
#include <atomic>

#include "../../src/util/LOG.hxx"
#include "../../src/services/GameClock.hxx"

TEST_CASE("I can schedule a deferred task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    WHEN("I schedule a callback to run in 3 seconds")
    {
      auto begin = std::chrono::system_clock::now();
      std::chrono::seconds dt;
      std::atomic<bool> isDone = false;
      clock.createDefferedTask(3s, [&begin, &dt, &isDone]() {
        dt = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin);
        isDone = true;
      });
      THEN("The task gets deffered to 3s later")
      {
        std::this_thread::sleep_for(3s);
        for (int i = 0; i < 5 && !isDone; i++)
        {
          std::this_thread::sleep_for(1s);
        }
        CAPTURE(isDone, dt);
        CHECK(isDone);
        CHECK(dt >= 3s);
      }
    }
  }
}

TEST_CASE("I can schedule multiple deffered tasks", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order {0, 0, 0};
      int rank = 1;
      clock.createDefferedTask(1s, [&order, &rank]() {
        order[0] = rank++;
      });
      clock.createDefferedTask(3s, [&order, &rank]() {
        order[2] = rank++;
      });
      clock.createDefferedTask(2s, [&order, &rank]() {
        order[1] = rank++;
      });
      THEN("The deffered tasks execute according to their delays")
      {
        for(int i = 0; i < 10; i++)
        {
          if(order[0] != 0 and order[1] != 0 and order[2] != 0) break;
          std::this_thread::sleep_for(1s);
        }
        CHECK(order == std::array{1, 2, 3});
      }
    }
  }
}
