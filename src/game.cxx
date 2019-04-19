#include "game.hxx"

#include "engine/engine.hxx"
#include "engine/eventManager.hxx"
#include "engine/uiManager.hxx"
#include "engine/windowManager.hxx"
#include "engine/audioMixer.hxx"
#include "engine/basics/camera.hxx"
#include "engine/basics/point.hxx"
#include "engine/basics/log.hxx"
#include "engine/ui/widgets/Image.hxx"
#include "engine/basics/settings.hxx"

#include <SDL.h>

bool Game::initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\n";
    LOG(LOG_ERROR) << "SDL Error:" << SDL_GetError();
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

  // initialize window manager
  WindowManager::instance().setWindowTitle(VERSION);
  return true;
}

void Game::splashscreen()
{
  int screenWidth = Settings::instance().settings.screenWidth;
  int screenHeight = Settings::instance().settings.screenHeight;
  bool mainMenuLoop = true;
  Image logo;
  Text versionText(VERSION);
  versionText.setPosition(screenWidth - versionText.getUiElementRect().w, screenHeight - versionText.getUiElementRect().h);

  Button newGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20, 200, 40});
  newGameButton.setText("New Game");
  newGameButton.registerCallbackFunction([]() { Engine::instance().newGame(); });

  Button loadGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20 + newGameButton.getUiElementRect().h * 2, 200, 40});
  loadGameButton.setText("Load Game");
  loadGameButton.registerCallbackFunction([]() { Engine::instance().loadGame("resources/save.cts"); });

  Button quitGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20 + loadGameButton.getUiElementRect().h * 4, 200, 40});
  quitGameButton.setText("Quit Game");
  quitGameButton.registerCallbackFunction(Signal::slot(this, &Game::shutdown));

  logo.setTextureID("Cytopia_Logo");
  logo.setVisibility(true);
  logo.setPosition(screenWidth / 2 - logo.getUiElementRect().w / 2, screenHeight / 4 - logo.getUiElementRect().h / 2);

  for (Uint8 opacity = 0; opacity < 255; opacity++)
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());
    logo.setOpacity(opacity);
    logo.draw();
    versionText.draw();
    newGameButton.draw();
    loadGameButton.draw();
    quitGameButton.draw();

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(WindowManager::instance().getRenderer());
    SDL_Delay(5);
  }
  while (mainMenuLoop)
  {
    logo.draw();
    versionText.draw();
    newGameButton.draw();
    loadGameButton.draw();
    quitGameButton.draw();
  }
  logo.setVisibility(false);
}

void Game::run()
{
  Timer benchmarkTimer;
  LOG() << VERSION;

  Engine &engine = Engine::instance();
  benchmarkTimer.start();

  LOG() << "Map initialized in " << benchmarkTimer.getElapsedTime() << "ms";
  Camera::centerScreenOnMapCenter();

  SDL_Event event;
  EventManager evManager;

  UIManager &uiManager = UIManager::instance();
  uiManager.init();

  AudioMixer audiomixer;
  audiomixer.playMusic();

  // FPS Counter variables
  const float fpsIntervall = 1.0; // interval the fps counter is refreshed in seconds.
  Uint32 fpsLastTime = SDL_GetTicks();
  Uint32 fpsFrames = 0;

  // Gameloop
  while (engine.isGameRunning())
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());

    evManager.checkEvents(event, engine);

    // render the tilemap
    engine.map->renderMap();

    // render the ui
    uiManager.drawUI();

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Render the Frame
    SDL_RenderPresent(WindowManager::instance().getRenderer());

    fpsFrames++;

    if (fpsLastTime < SDL_GetTicks() - fpsIntervall * 1000)
    {
      fpsLastTime = SDL_GetTicks();
      uiManager.setFPSCounterText(std::to_string(fpsFrames) + " FPS");
      fpsFrames = 0;
    }

    SDL_Delay(1);
  }
}

void Game::shutdown()
{
  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}
