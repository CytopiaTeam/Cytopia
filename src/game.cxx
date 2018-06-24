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

  Point clickCoords, mouseCoords;
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

    if (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        engine.quitGame();
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
          uiManager.toggleGroupVisibility(2);
          break;

        case SDLK_0:
          engine.toggleLayer(Engine::LAYER_GRID);
          break;

        case SDLK_1:
          engine.toggleLayer(Engine::LAYER_FLOOR);
          break;

        case SDLK_2:
          engine.toggleLayer(Engine::LAYER_BUILDINGS);
          break;

        case SDLK_3:
          engine.toggleLayer(Engine::LAYER_SELECTION);
          break;

        case SDLK_f:
          engine.toggleFullScreen();
          break;
        case SDLK_b:
          LOG() << "Starting elevation Benchmark!";
          LOG().timerStart();
          for (int i = 0; i <= Resources::settings.maxElevationHeight; i++)
          {
            engine.increaseHeightOfCell(Point{64, 64, 0, 0});
          }
          LOG().timerEnd();
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        // hacky event handling for now
        if (evManager.checkEvents(event))
        {
          // events are handled -> done
          break;
        }

        mouseCoords = Point{event.button.x, event.button.y, 0, 0};
        clickCoords = Resources::convertScreenToIsoCoordinates(mouseCoords);
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          if (engine.isPointWithinBoundaries(clickCoords))
          {
            if (Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE)
              engine.increaseHeightOfCell(clickCoords);
            else if (Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER)
            {
              engine.decreaseHeightOfCell(clickCoords);
            }
            else
              LOG() << "CLICKED - Iso Coords: " << clickCoords.x << ", " << clickCoords.y;
          }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          engine.centerScreenOnPoint(clickCoords);
        }
        break;

      case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0)
        {
          engine.increaseZoomLevel();
        }
        else if (event.wheel.y < 0)
        {
          engine.decreaseZoomLevel();
        }
        break;

      default:
        break;
      }
    }

    // render the tilemap
    engine.render();
    
    // render the ui
    uiManager.drawUI();

    // reset renderer color back to black
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Render the Frame
    SDL_RenderPresent(_renderer);
    SDL_Delay(1);
  }
}
