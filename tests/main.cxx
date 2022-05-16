#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <SDL2/SDL.h>

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
