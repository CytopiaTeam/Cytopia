#include <iostream>

#include "Game.hxx"
#include "util/Exception.hxx"

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

  #ifdef __WIN__
  /**
   * @todo register signal handler for windows
   */
  #else
  /* Register handler for Segmentation Fault */
  signal(SIGSEGV, SIG_handler);
  signal(SIGINT, SIG_handler);
  signal(SIGTERM, SIG_handler);
  #endif

  try
  {
    return protected_main(argc, argv);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Caught unhandled exception:\n";
    std::cerr << " - what(): " << e.what() << '\n';
  }
  catch (...)
  {
    std::cerr << "Caught unknown exception\n";
  }

  return EXIT_FAILURE;
}
