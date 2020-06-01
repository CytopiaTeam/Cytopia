#include <catch2/catch.hpp>
#include <array>
#include <atomic>
#include <thread>

#include "../../src/util/LOG.hxx"
#include "../../src/services/GameClock.hxx"

void tick_clock(unsigned long ticks, GameClock &clock)
{
  while (ticks-- > 0U)
  {
    clock.tick();
    std::this_thread::sleep_for(100ms);
  }
}

TEST_CASE("I can schedule a real time task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 70, std::ref(clock));

    WHEN("I schedule a callback to run in 3 seconds")
    {
      auto begin = std::chrono::system_clock::now();
      std::chrono::milliseconds dt;
      std::atomic<bool> isDone = false;
      clock.addRealTimeClockTask(
          [&begin, &dt, &isDone]() {
            dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin);
            isDone = true;
          },
          3s);
      tickingThread.join();

      THEN("The task gets called after 3s")
      {
        CAPTURE(isDone, dt);
        CHECK(isDone);
        CHECK(((dt >= 3000ms) && (3100ms >= dt)));
      }
    }
  }
}

TEST_CASE("I can schedule multiple real time tasks", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 35, std::ref(clock));

    WHEN("I schedule multiple real tasks with different delays")
    {
      std::array order{0, 0, 0};
      int rank = 1;

      clock.addRealTimeClockTask([&order, &rank]() { order[0] = rank++; }, 1s);
      clock.addRealTimeClockTask([&order, &rank]() { order[2] = rank++; }, 3s);
      clock.addRealTimeClockTask([&order, &rank]() { order[1] = rank++; }, 2s);
      tickingThread.join();

      THEN("The deferred tasks execute according to their delays")
      {
        CHECK(order == std::array{1, 2, 3});
      }
    }
  }
}

TEST_CASE("I can schedule real time repeating task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 65, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addRealTimeClockTask([&counter]() { counter++; }, 100ms, 1s);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 7); }
    }
  }
}

TEST_CASE("I can schedule multiple real time tasks and remove some", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 65, std::ref(clock));

    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order{0, 0, 0, 0};

      auto task1 = clock.addRealTimeClockTask([&order]() { order[0]++; }, 1s, 1s);
      auto task2 = clock.addRealTimeClockTask([&order]() { order[1]++; }, 1s, 1s);
      auto task3 = clock.addRealTimeClockTask([&order]() { order[2]++; }, 1s, 1s);
      auto task4 = clock.addRealTimeClockTask([&order]() { order[3]++; }, 3s, 3s);

      std::this_thread::sleep_for(3100ms);
      bool isRemoved3 = clock.removeClockTask(task3);
      bool isRemoved2 = clock.removeClockTask(task2);
      tickingThread.join();

      THEN("The repeating task executes exact number of times. The removed tasks stop executing")
      {
        CHECK(isRemoved2 == true);
        CHECK(isRemoved3 == true);
        CHECK(order == std::array{6, 3, 3, 2});
      }
    }
  }
}

TEST_CASE("I can schedule a game time task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 90, std::ref(clock));

    WHEN("I schedule a callback to run after 2 game minutes (4s)")
    {
      auto begin = std::chrono::system_clock::now();
      std::chrono::milliseconds dt;
      std::atomic<bool> isDone = false;
      clock.addGameTimeClockTask(
          [&begin, &dt, &isDone]() {
            dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin);
            isDone = true;
          },
          2 * GameClock::GameMinute);
      tickingThread.join();

      THEN("The task gets called after 4s")
      {
        CAPTURE(isDone, dt);
        CHECK(isDone);
        CHECK(dt >= 4000ms);
      }
    }
  }
}

TEST_CASE("I can schedule multiple game time tasks", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 90, std::ref(clock));

    WHEN("I schedule multiple game tasks with different delays")
    {
      std::array order{0, 0, 0};
      int rank = 1;

      clock.addGameTimeClockTask([&order, &rank]() { order[0] = rank++; }, GameClock::GameMinute);
      clock.addGameTimeClockTask([&order, &rank]() { order[2] = rank++; }, 3 * GameClock::GameMinute);
      clock.addGameTimeClockTask([&order, &rank]() { order[1] = rank++; }, 2 * GameClock::GameMinute);
      tickingThread.join();

      THEN("The deferred tasks execute according to their delays") { CHECK(order == std::array{1, 2, 3}); }
    }
  }
}

TEST_CASE("I can schedule game time repeating task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 110, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addGameTimeClockTask([&counter]() { counter++; }, GameClock::GameMinute, GameClock::GameMinute);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 4); }
    }
  }
}

TEST_CASE("I can schedule multiple game time tasks and remove some", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 150, std::ref(clock));

    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order{0, 0, 0, 0};

      auto task1 = clock.addGameTimeClockTask([&order]() { order[0]++; }, GameClock::GameMinute, GameClock::GameMinute);
      auto task2 = clock.addGameTimeClockTask([&order]() { order[1]++; }, GameClock::GameMinute, GameClock::GameMinute);
      auto task3 = clock.addGameTimeClockTask([&order]() { order[2]++; }, GameClock::GameMinute, GameClock::GameMinute);
      auto task4 = clock.addGameTimeClockTask([&order]() { order[3]++; }, GameClock::GameMinute, 2 * GameClock::GameMinute);

      std::this_thread::sleep_for(6500ms);
      bool isRemoved3 = clock.removeClockTask(task3);
      bool isRemoved2 = clock.removeClockTask(task2);
      tickingThread.join();

      THEN("The repeating task executes exact number of times. The removed tasks stop executing")
      {
        CHECK(isRemoved2 == true);
        CHECK(isRemoved3 == true);
        CHECK(order == std::array{6, 2, 2, 3});
      }
    }
  }
}

TEST_CASE("I can speed up game time", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 27, std::ref(clock));

    WHEN("I schedule game task and speed up game time 4 times")
    {
      int counter = {0};

      clock.addGameTimeClockTask([&counter]() { counter++; }, GameClock::GameMinute, GameClock::GameMinute);
      // Game minute will be scaled from 2s to 500ms
      clock.setGameClockSpeed(4.0f);
      tickingThread.join();

      THEN("The repeating task executes exact number of times. The removed tasks stop executing")
      {
        CHECK(counter == 4);
      }
    }
  }
}

TEST_CASE("I can slow down game time", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 130, std::ref(clock));

    WHEN("I schedule game task and slow down game time 2 times")
    {
      int counter = {0};

      clock.addGameTimeClockTask([&counter]() { counter++; }, GameClock::GameMinute, GameClock::GameMinute);
      // Game minute will be scaled from 2s to 4s
      clock.setGameClockSpeed(0.5f);
      tickingThread.join();

      THEN("The repeating task executes exact number of times. The removed tasks stop executing") { CHECK(counter == 2); }
    }
  }
}

TEST_CASE("I can schedule multiple real/game time tasks and clear them all", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameService::ServiceTuple ctx = GameService::ServiceTuple{};
    GameClock clock(ctx);
    auto tickingThread = std::thread(tick_clock, 90, std::ref(clock));

    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order{0, 0, 0};

      clock.addGameTimeClockTask([&order]() { order[0]++; }, GameClock::GameMinute, GameClock::GameMinute);
      clock.addRealTimeClockTask([&order]() { order[2]++; }, 1s, 1s);
      clock.addGameTimeClockTask([&order]() { order[1]++; }, 2 * GameClock::GameMinute);
      std::this_thread::sleep_for(5500ms);
      clock.clear();
      tickingThread.join();

      THEN("The deferred tasks execute according to their delays") { CHECK(order == std::array{1, 0, 5}); }
    }
  }
}