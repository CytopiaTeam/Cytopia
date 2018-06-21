#include "game.hxx"
#include "engine/basics/log.hxx"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

bool initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
    return false;
  }

  if (TTF_Init() == -1)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL_TTF\nSDL Error:" << TTF_GetError();
    return false;
  }

  if (Mix_Init(MIX_INIT_MP3) == -1)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL_Mixer\nSDL Error:" << Mix_GetError();
    return false;
  }

  return true;
}

void shutdown()
{
  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}

int main(int, char **)
{
  // initialize all systems and globals
  auto const initialized = initialize();

  if (initialized)
  {
    // run the game
    run();
  }
  else
  {
    return EXIT_FAILURE;
  }

  // shutdown all systems and clean-up stuff
  shutdown();

  return EXIT_SUCCESS;
}
