#include <iostream>

#include "Game.hxx"
#include "Exception.hxx"
#include "LOG.hxx"

int protected_main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  bool quitGame = false;

  // add commandline parameter to skipMenu
  auto has_args = [argv, argc] (const std::string &param) {
    for (int i = 1; i < argc; ++i)
      if (param == argv[i])
        return i;

    LOG(LOG_DEBUG) << "Unknown game option " << param;
    return 0;
  };

  bool skipMenu = has_args("--skipMenu");
  uint32_t videoOpt = has_args("--video");
  const char *videoDriver = nullptr;
  if (videoOpt) {
    videoDriver = argv[videoOpt + 1];
  }

  LOG(LOG_DEBUG) << "Launching Cytopia";

  Cytopia::Game game;

  LOG(LOG_DEBUG) << "Initializing Cytopia";

  if (!game.initialize(videoDriver))
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
  systemSetupCrashHandler();

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
