#include "game.hxx"

#include <iostream>

int protected_main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  Game game;

  if (!game.initialize())
  {
    return EXIT_FAILURE;
  }

  game.splashscreen();
  game.run();
  game.shutdown();

  return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
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
