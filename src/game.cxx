#include "game.hxx"

#include "engine/engine.hxx"
#include "engine/eventManager.hxx"
#include "engine/uiManager.hxx"
#include "engine/windowManager.hxx"
#include "engine/basics/camera.hxx"
#include "engine/basics/point.hxx"
#include "engine/basics/log.hxx"
#include "engine/ui/widgets/Image.hxx"
#include "engine/basics/settings.hxx"
#include <noise.h>
#include <SDL.h>

#ifndef DISABLE_SDL2_MIXER
  #include "engine/audioMixer.hxx"
#endif

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

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

#ifndef DISABLE_SDL2_MIXER
  if (Mix_Init(MIX_INIT_MP3) == -1)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL_Mixer\nSDL Error:" << Mix_GetError();
    return false;
  }
#endif

  // initialize window manager
  WindowManager::instance().setWindowTitle(VERSION);
  return true;
}

void Game::mainMenu()
{
  SDL_Event event;

  int screenWidth = Settings::instance().settings.screenWidth;
  int screenHeight = Settings::instance().settings.screenHeight;
  bool mainMenuLoop = true;

  Image logo;
  logo.setTextureID("Cytopia_Logo");
  logo.setVisibility(true);
  logo.setPosition(screenWidth / 2 - logo.getUiElementRect().w / 2, screenHeight / 4 - logo.getUiElementRect().h / 2);

  Text versionText(VERSION);
  versionText.setPosition(screenWidth - versionText.getUiElementRect().w, screenHeight - versionText.getUiElementRect().h);

  Button newGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20, 200, 40});
  newGameButton.setText("New Game");
  newGameButton.setUIElementID("newgame");

  Button loadGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20 + newGameButton.getUiElementRect().h * 2, 200, 40});
  loadGameButton.setText("Load Game");
  loadGameButton.registerCallbackFunction([]() { Engine::instance().loadGame("resources/save.cts"); });

  Button quitGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20 + loadGameButton.getUiElementRect().h * 4, 200, 40});
  quitGameButton.setText("Quit Game");
  quitGameButton.registerCallbackFunction([]() { Engine::instance().quitGame(); });

  // store elements in vector
  std::vector<UiElement *> uiElements;
  uiElements.push_back(&newGameButton);
  uiElements.push_back(&loadGameButton);
  uiElements.push_back(&quitGameButton);
  uiElements.push_back(&logo);
  uiElements.push_back(&versionText);

  UiElement *m_lastHoveredElement = nullptr;

  // fade in Logo
  for (Uint8 opacity = 0; opacity < 255; opacity++)
  {
    // break the loop if an event occurs
    if (SDL_PollEvent(&event))
    {
      if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN)
      {
        logo.setOpacity(SDL_ALPHA_OPAQUE);
        break;
      }
    }
    SDL_RenderClear(WindowManager::instance().getRenderer());
    logo.setOpacity(opacity);

    for (const auto &element : uiElements)
    {
      element->draw();
    }

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(WindowManager::instance().getRenderer());
    SDL_Delay(5);
  }

  while (mainMenuLoop)
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());

    while (SDL_PollEvent(&event) != 0)
    {
      for (const auto &it : uiElements)
      {
        switch (event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
          it->onMouseButtonDown(event);
          break;
        case SDL_MOUSEBUTTONUP:

          if (it->onMouseButtonUp(event))
          {
            mainMenuLoop = false;
          }
          break;
        case SDL_MOUSEMOTION:
          it->onMouseMove(event);

          // if the mouse cursor left an element, we're not hovering any more and we need to reset the pointer to the last hovered
          if (m_lastHoveredElement && !m_lastHoveredElement->isMouseOverHoverableArea(event.button.x, event.button.y))
          {
            m_lastHoveredElement->onMouseLeave(event);
            m_lastHoveredElement = nullptr;
          }

          // if the element we're hovering over is not the same as the stored "lastHoveredElement", update it
          if (it->isMouseOverHoverableArea(event.button.x, event.button.y) && it != m_lastHoveredElement)
          {
            it->onMouseMove(event);

            if (m_lastHoveredElement != nullptr)
            {
              m_lastHoveredElement->onMouseLeave(event);
            }
            m_lastHoveredElement = it;
            it->onMouseEnter(event);
          }
          break;
        }
      }
    }

    for (const auto &element : uiElements)
    {
      element->draw();
    }

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(WindowManager::instance().getRenderer());
  }
}

void Game::run()
{
  Timer benchmarkTimer;
  LOG() << VERSION;

  benchmarkTimer.start();
  Engine &engine = Engine::instance();

  LOG() << "Map initialized in " << benchmarkTimer.getElapsedTime() << "ms";
  Camera::centerScreenOnMapCenter();

  SDL_Event event;
  EventManager evManager;

  UIManager &uiManager = UIManager::instance();
  uiManager.init();
#ifndef DISABLE_SDL2_MIXER
  AudioMixer audiomixer;
  audiomixer.playMusic();
#endif

  // FPS Counter variables
  const float fpsIntervall = 1.0; // interval the fps counter is refreshed in seconds.
  Uint32 fpsLastTime = SDL_GetTicks();
  Uint32 fpsFrames = 0;

  // Gameloop
  while (engine.isGameRunning())
  {
#ifdef MICROPROFILE_ENABLED
      MICROPROFILE_SCOPEI ("Map", "Gameloop", MP_GREEN);
#endif
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

#ifdef MICROPROFILE_ENABLED
      MicroProfileFlip(nullptr);
#endif

  }
}

void Game::shutdown()
{
  TTF_Quit();

#ifndef DISABLE_SDL2_MIXER
  Mix_Quit();
#endif

  SDL_Quit();
}
