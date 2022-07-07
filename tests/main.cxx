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
void start_test()
{
  ThreadWorkers::instance().initialize(4);
  ThreadWorkers::instance().execute([] {
    while (!testFinished) {
      ResourcesManager::instance().performDelayedActions();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
}

void finish_test()
{
  testFinished = true;
  ThreadWorkers::instance().shutdown();
}
