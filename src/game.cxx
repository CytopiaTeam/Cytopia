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

  unsigned currentTick = 0;
  unsigned lastTick = 0;
  unsigned elapsedTicks = 0;

  currentTick = lastTick = SDL_GetTicks();

  unsigned fpsTimer = 0;
  unsigned fpsFrameCount = 0;
  unsigned fpsAccurate = 0;
  unsigned fpsInstantaneous = 0;

  // Gameloop
  while (engine.gameIsRunning())
  {

    lastTick = currentTick;
    currentTick = SDL_GetTicks();
    elapsedTicks = currentTick - lastTick;

    float elapsedTime = elapsedTicks / 1000.f;

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

    ++fpsFrameCount;
    fpsTimer += elapsedTicks;
    if (fpsTimer >= 10000) {
      fpsAccurate = fpsFrameCount;
      fpsFrameCount = 0;
      fpsTimer = 0;
        LOG() << "Accurate: " << fpsAccurate << "fps sampled over 1000 Frames";
    }
    //if (elapsedTicks)
    //{
    //  fpsInstantaneous = 1000 / elapsedTicks;
    //  LOG() << "Instant: " << fpsAccurate << "fps";
    //}

    //SDL_Delay(1);
  }
}
