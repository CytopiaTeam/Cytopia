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
  LOG() << VERSION;
  LOG().timerStart();
  Resources::init();

  Engine &engine = Engine::Instance();
  LOG().timerEnd();
  LOG() << "Tile Matrix initialized";

  SDL_Event event;
  EventManager evManager = EventManager();

  UIManager &uiManager = UIManager::Instance();
  uiManager.init();

  auto *_renderer = Resources::getRenderer();
  auto *_window = Resources::getWindow();
  AudioMixer audiomixer;
  audiomixer.playMusic();

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
    SDL_Delay(1);
  }
}
