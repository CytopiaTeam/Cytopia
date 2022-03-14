#include <catch.hpp>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>

#include "../../src/util/LOG.hxx"
#include "../../src/services/GameClock.hxx"

void tick_clock(std::chrono::microseconds duration, GameClock &clock)
{
  auto start = std::chrono::high_resolution_clock::now();

  while (duration > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start))
  {
    clock.tick();
    std::this_thread::sleep_for(10ms);
  }

  clock.tick();
}

TEST_CASE("I can schedule a real time task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 7000ms, std::ref(clock));

    WHEN("I schedule a callback to run in 3 seconds")
    {
      auto begin = std::chrono::system_clock::now();
      std::chrono::milliseconds dt;
      std::atomic<bool> isDone = false;
      clock.addRealTimeClockTask(
          [&begin, &dt, &isDone]() {
            dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin);
            isDone = true;
            return false;
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
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 3500ms, std::ref(clock));

    WHEN("I schedule multiple real tasks with different delays")
    {
      std::array order{0, 0, 0};
      int rank = 1;

      clock.addRealTimeClockTask(
          [&order, &rank]() {
            order[0] = rank++;
            return false;
          },
          1s);
      clock.addRealTimeClockTask(
          [&order, &rank]() {
            order[2] = rank++;
            return false;
          },
          3s);
      clock.addRealTimeClockTask(
          [&order, &rank]() {
            order[1] = rank++;
            return false;
          },
          2s);
      tickingThread.join();

      THEN("The deferred tasks execute according to their delays") { CHECK(order == std::array{1, 2, 3}); }
    }
  }
}

TEST_CASE("I can schedule real time repeating task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 6900ms, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addRealTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          100ms, 1s);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 7); }
    }
  }
}

TEST_CASE("I can schedule real time repeating task to trigger immediately", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 2900ms, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addRealTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          0s, 1s);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 3); }
    }
  }
}

TEST_CASE("I can schedule real time task to trigger immediately", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 2900ms, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addRealTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          0s, 0s);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 1); }
    }
  }
}

TEST_CASE("I can schedule game time repeating task to trigger immediately", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 6900ms, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addGameTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          0, GameClock::GameMinute);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 3); }
    }
  }
}

TEST_CASE("I can schedule game time task to trigger immediately", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 6900ms, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addGameTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          0, 0);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 1); }
    }
  }
}

TEST_CASE("I can schedule multiple real time tasks and remove some", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 6500ms, std::ref(clock));

    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order{0, 0, 0, 0};

      auto task1 = clock.addRealTimeClockTask(
          [&order]() {
            order[0]++;
            return false;
          },
          1s, 1s);
      auto task2 = clock.addRealTimeClockTask(
          [&order]() {
            order[1]++;
            return false;
          },
          1s, 1s);
      auto task3 = clock.addRealTimeClockTask(
          [&order]() {
            order[2]++;
            return false;
          },
          1s, 1s);
      auto task4 = clock.addRealTimeClockTask(
          [&order]() {
            order[3]++;
            return false;
          },
          3s, 3s);

      std::this_thread::sleep_for(3500ms);
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
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 9000ms, std::ref(clock));

    WHEN("I schedule a callback to run after 2 game minutes (4s)")
    {
      auto begin = std::chrono::system_clock::now();
      std::chrono::milliseconds dt;
      std::atomic<bool> isDone = false;
      clock.addGameTimeClockTask(
          [&begin, &dt, &isDone]() {
            dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin);
            isDone = true;
            return false;
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
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 9000ms, std::ref(clock));

    WHEN("I schedule multiple game tasks with different delays")
    {
      std::array order{0, 0, 0};
      int rank = 1;

      clock.addGameTimeClockTask(
          [&order, &rank]() {
            order[0] = rank++;
            return false;
          },
          GameClock::GameMinute);
      clock.addGameTimeClockTask(
          [&order, &rank]() {
            order[2] = rank++;
            return false;
          },
          3 * GameClock::GameMinute);
      clock.addGameTimeClockTask(
          [&order, &rank]() {
            order[1] = rank++;
            return false;
          },
          2 * GameClock::GameMinute);
      tickingThread.join();

      THEN("The deferred tasks execute according to their delays") { CHECK(order == std::array{1, 2, 3}); }
    }
  }
}

TEST_CASE("I can schedule game time repeating task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 11000ms, std::ref(clock));

    WHEN("I schedule repeating real time task")
    {
      int counter = 0;

      clock.addGameTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      tickingThread.join();

      THEN("The repeating task executes exact number of times") { CHECK(counter == 4); }
    }
  }
}

TEST_CASE("I can schedule multiple game time tasks and remove some", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 15000ms, std::ref(clock));

    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order{0, 0, 0, 0};

      auto task1 = clock.addGameTimeClockTask(
          [&order]() {
            order[0]++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      auto task2 = clock.addGameTimeClockTask(
          [&order]() {
            order[1]++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      auto task3 = clock.addGameTimeClockTask(
          [&order]() {
            order[2]++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      auto task4 = clock.addGameTimeClockTask(
          [&order]() {
            order[3]++;
            return false;
          },
          GameClock::GameMinute, 2 * GameClock::GameMinute);

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
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 2900ms, std::ref(clock));

    WHEN("I schedule game task and speed up game time 4 times")
    {
      int counter = {0};

      clock.addGameTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      // Game minute will be scaled from 2s to 500ms
      clock.setGameClockSpeed(4.0f);
      tickingThread.join();

      THEN("The repeating task executes exact number of times. The removed tasks stop executing") { CHECK(counter == 4); }
    }
  }
}

TEST_CASE("I can slow down game time", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 13000ms, std::ref(clock));

    WHEN("I schedule game task and slow down game time 2 times")
    {
      int counter = {0};

      clock.addGameTimeClockTask(
          [&counter]() {
            counter++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
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
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 9000ms, std::ref(clock));

    WHEN("I schedule multiple tasks with different delays")
    {
      std::array order{0, 0, 0};

      clock.addGameTimeClockTask(
          [&order]() {
            order[0]++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      clock.addRealTimeClockTask(
          [&order]() {
            order[2]++;
            return false;
          },
          1s, 1s);
      clock.addGameTimeClockTask(
          [&order]() {
            order[1]++;
            return false;
          },
          2 * GameClock::GameMinute);
      std::this_thread::sleep_for(5500ms);
      clock.clear();
      tickingThread.join();

      THEN("The deferred tasks execute according to their delays") { CHECK(order == std::array{1, 0, 5}); }
    }
  }
}

TEST_CASE("I try to remove non existing task", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;

    WHEN("I schedule one task and try to remove non existing one")
    {
      auto res = clock.removeClockTask(0xFFFFFFFF);

      THEN("The task cannot be removed and false is returned") { CHECK(res == false); }
    }
  }
}

TEST_CASE("I can remove myself in callback", "[engine][clock]")
{
  GIVEN("I have a Game Clock running")
  {
    GameClock clock;
    auto tickingThread = std::thread(tick_clock, 500ms, std::ref(clock));

    WHEN("I schedule game task and remove it during callback")
    {
      int counter = {0};

      clock.addGameTimeClockTask(
          [&counter]() {
            if (counter == 2)
            {
              // remove itself
              return true;
            }

            counter++;
            return false;
          },
          GameClock::GameMinute, GameClock::GameMinute);
      // Game minute will be scaled from 2s to 100ms
      clock.setGameClockSpeed(20.f);
      tickingThread.join();

      THEN("The repeating must be executed only 2 times than it removes itself.") { CHECK(counter == 2); }
    }
  }
}
