#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <SDL2/SDL.h>

#include <ThreadWorkers.hxx>
#include <ResourcesManager.hxx>
#include <WindowManager.hxx>

/*
 * This file should remain empty.
 * To add tests, refer to ./Example.cxx
 */

SDL_AssertState TestAssertionHandler(const SDL_AssertData *, void *) { return SDL_ASSERTION_IGNORE; }

struct test_init
{
  test_init()
  {
    /* We need to handle assertion failures, otherwise SDL will open a window */
    SDL_SetAssertionHandler((SDL_AssertionHandler)TestAssertionHandler, 0);
  }

} test_init_instance;

bool testFinished = false;
ThreadWorkers test_mt;
void start_test()
{
  test_mt.initialize(4);
  test_mt.execute([] {
    while (!testFinished) {
      ResourcesManager::instance().performDelayedActions();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
}

void finish_test()
{
  testFinished = true;
  test_mt.shutdown();
}
