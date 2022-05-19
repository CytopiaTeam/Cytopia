#include <iostream>

#include "Game.hxx"
#include "MainMenu.hxx"
#include "Exception.hxx"
#include "LOG.hxx"
#include "engine/WindowManager.hxx"
#include <UIManager.hxx>
#include "SimpleOpt.h"
#include "ParseCli.hxx"

#include <SDL.h>
#include <SDL_ttf.h>

bool initialize()
{
  if (SDL_Init(0) != 0)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL";
    LOG(LOG_ERROR) << "SDL Error: " << SDL_GetError();
    return false;
  }

  const char *videoDriver = nullptr;
  if (Settings::instance().videoDriver != "Default")
    videoDriver = Settings::instance().videoDriver.c_str();

  if (SDL_VideoInit(videoDriver) != 0)
  {
    LOG(LOG_ERROR) << "Unknown video driver " << videoDriver;
    int nbDriver = SDL_GetNumRenderDrivers();
    for (int i = 0; i < nbDriver; i++)
    {
      SDL_RendererInfo info;
      SDL_GetRenderDriverInfo(i, &info);
      LOG(LOG_ERROR) << "Found driver " << i << ": " << (info.name ? info.name : "Invalid driver")
                     << " with flags=" << info.flags;
    }
    return false;
  }

  if (TTF_Init() == -1)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL_TTF";
    LOG(LOG_ERROR) << "SDL Error: " << TTF_GetError();
    return false;
  }

  // initialize window manager
  WindowManager::instance().setWindowTitle(VERSION);
  return true;
}

int protected_main(int argc, char **argv)
{
  ParseCli cli;
  if (!cli.ProcessCommandLine(argc, argv))
    return EXIT_FAILURE;

  LOG(LOG_DEBUG) << "Launching Cytopia";

  Cytopia::Game game;

  LOG(LOG_DEBUG) << "Initializing Cytopia";

  if (!initialize())
    return EXIT_FAILURE;
  else
    LOG(LOG_DEBUG) << "DONE Cytopia";

  bool startGame = true;
  if (!Settings::instance().skipMenu)
  {
    startGame = mainMenu();
  }

  if (startGame)
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