#include <iostream>

#include "Game.hxx"
#include "Exception.hxx"
#include "LOG.hxx"
#include "engine/basics/Settings.hxx"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
#include <signal.h>
void SIG_handler(int signal);
#endif

SDL_AssertState AssertionHandler(const SDL_AssertData *, void *);

class Initializer {
public:
  Initializer() {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
      throw CytopiaError{TRACE_INFO "Failed to initialize SDL: " + std::string(SDL_GetError())};
    }

    if (TTF_Init() == -1)
    {
      throw CytopiaError{TRACE_INFO "Failed to initialize SDL_TTF: " + std::string(TTF_GetError())};
    }
  }
  ~Initializer() {
    TTF_Quit();
    SDL_Quit();
  }
};

int protected_main(int argc, char **argv)
{
  bool quitGame = false;
  Settings & settings = Settings::instance();
  settings.parse_args(argc, argv);

  LOG(LOG_DEBUG) << "Launching Cytopia";

  Initializer initializer;
  Game game;

  LOG(LOG_DEBUG) << "Initializing Cytopia";

  if (!game.initialize())
    return EXIT_FAILURE;

  if (!settings.skipMenu)
  {
    quitGame = game.mainMenu();
  }

  if (!quitGame)
  {
    LOG(LOG_DEBUG) << "Running the Game";
    game.run();
  }

  LOG(LOG_DEBUG) << "Closing the Game";
  game.shutdown();

  return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
  /* Register handler for Segmentation Fault, Interrupt, Terminate */
  signal(SIGSEGV, SIG_handler);
  signal(SIGINT, SIG_handler);
  signal(SIGTERM, SIG_handler);
#endif

  /* All SDL2 Assertion failures must be handled
   * by our handler */
  SDL_SetAssertionHandler(AssertionHandler, 0);

  try
  {
    return protected_main(argc, argv);
  }
  catch (std::exception &e)
  {
    LOG(LOG_ERROR) << e.what();
  }
  catch (...)
  {
    LOG(LOG_ERROR) << "Caught unknown exception";
  }

  return EXIT_FAILURE;
}
