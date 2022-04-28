#include <iostream>

#include "Game.hxx"
#include "Exception.hxx"
#include "LOG.hxx"

#include <csignal>
#ifndef __ANDROID__
void SIG_handler(int signal);
#endif

SDL_AssertState AssertionHandler(const SDL_AssertData *, void *);

int protected_main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  bool skipMenu = false;
  bool quitGame = false;

  // add commandline parameter to skipMenu
  for (int i = 1; i < argc; ++i)
  {
    if (std::string(argv[i]) == "--skipMenu")
    {
      skipMenu = true;
    }
  }

  LOG(LOG_DEBUG) << "Launching Cytopia";

  Cytopia::Game game;

  LOG(LOG_DEBUG) << "Initializing Cytopia";

  if (!game.initialize())
    return EXIT_FAILURE;

  if (!skipMenu)
  {
    quitGame = game.mainMenu();
  }

  if (!quitGame)
  {
    LOG(LOG_DEBUG) << "Running the Game";
    game.run(skipMenu);
  }

  LOG(LOG_DEBUG) << "Closing the Game";
  game.shutdown();

  return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
#ifndef __ANDROID__
  /* Register handler for Segmentation Fault, Interrupt, Terminate */
  signal(SIGSEGV, SIG_handler);
  signal(SIGINT, SIG_handler);
  signal(SIGTERM, SIG_handler);
  /* All SDL2 Assertion failures must be handled
   * by our handler */
  SDL_SetAssertionHandler(AssertionHandler, 0);
#endif

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
