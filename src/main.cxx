#include <iostream>

#include "Game.hxx"
#include "util/Exception.hxx"

#ifdef _WIN32
#else
#include <execinfo.h>
#include <signal.h>
void SIG_handler(int signal);
#endif
SDL_AssertState AssertionHandler(const SDL_AssertData *, void *);

int protected_main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  bool skipMenu = false;

  // add commandline parameter to skipMenu
  for (int i = 1; i < argc; ++i)
  {
    if (std::string(argv[i]) == "--skipMenu")
    {
      skipMenu = true;
    }
  }
  Game game;

  if (!game.initialize())
  {
    return EXIT_FAILURE;
  }

  if (!skipMenu)
  {
    game.mainMenu();
  }
  LOG(LOG_DEBUG) << "Game::run";
  game.run(skipMenu);
  game.shutdown();

  return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{

  #ifdef _WIN32
  /**
   * @todo register signal handler for windows
   */
  #else
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
    LOG(LOG_EXCEPTION) << e.what();
  }
  catch (...)
  {
    LOG(LOG_EXCEPTION) << "Caught unknown exception";
  }

  return EXIT_FAILURE;
}
