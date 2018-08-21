#include "SDL2/SDL.h"

#include "engine/engine.hxx"
#include "engine/eventManager.hxx"
#include "engine/uiManager.hxx"
#include "engine/audioMixer.hxx"
#include "engine/basics/point.hxx"
#include "engine/basics/resources.hxx"
#include "engine/basics/log.hxx"

void run()
{
  Timer benchmarkTimer;
  LOG() << VERSION;
  Resources::init();

  benchmarkTimer.start();
  Engine &engine = Engine::Instance();
  LOG() << "Tile Matrix initialized in " << benchmarkTimer.getDeltaTime() << "ms";

  SDL_Event event;
  EventManager evManager;

  UIManager &uiManager = UIManager::Instance();
  uiManager.init();

  auto *_renderer = Resources::getRenderer();
  auto *_window = Resources::getWindow();
  AudioMixer audiomixer;
  audiomixer.playMusic();

  // FPS Counter variables
  const float fpsIntervall = 1.0; // interval the fps counter is refreshed in seconds.
  Uint32 fpsLastTime = SDL_GetTicks();
  Uint32 fpsFrames = 0;

  // Gameloop
  while (engine.gameIsRunning())
  {
    SDL_RenderClear(_renderer);

    evManager.checkEvents(event, engine);

    // render the tilemap
    engine.render();

    // render the ui
    uiManager.setButtonState();
    uiManager.drawUI();

    // reset renderer color back to black
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Render the Frame
    SDL_RenderPresent(_renderer);

    fpsFrames++;
    if (fpsLastTime < SDL_GetTicks() - fpsIntervall * 1000)
    {
      fpsLastTime = SDL_GetTicks();
      uiManager.setFPSCounterText(std::to_string(fpsFrames) + " FPS");
      fpsFrames = 0;
    }

    //SDL_Delay(1);
  }
}
