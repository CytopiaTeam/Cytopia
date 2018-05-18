#include "game.hxx"


int main(int, char**)
{
  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;
  double deltaTime = 0;

  Resources::readJSONFile();

  int screen_height = 800;
  int screen_width = 600;
  bool fullscreen = false;

  Window window("Isometric Engine", screen_height, screen_width);
  Engine& engine = Engine::Instance();
  Point clickCoords, mouseCoords;
  SDL_Event event;

  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  
  // Gameloop
  while (!window.isClosed()){
    SDL_RenderClear(_renderer);


    if ( SDL_PollEvent(&event) )
    {
      switch (event.type)
      {
      case SDL_QUIT:
        window.close();
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) 
        {
          case SDLK_ESCAPE:
            window.close();
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
          case SDLK_j:
            Resources::generateJSONFile();
            break;
          case SDLK_e:
            printf("Toggling Edit Mode\n");
            editMode = !editMode;
            break;
          case SDLK_f:
            window.toggleFullScreen();
            break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        mouseCoords.setCoords(event.button.x, event.button.y);
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        clickCoords = Resources::convertScreenToIsoCoordinates(mouseCoords);
        if ( event.button.button == SDL_BUTTON_LEFT )
        {
          if ( engine.checkBoundaries(clickCoords) )
          {
            if (editMode)
              engine.increaseHeight(clickCoords);
            else
              printf("CLICKED - Iso Coords: %d , %d\n", clickCoords.getX(), clickCoords.getY());
          }
        }
        if ( event.button.button == SDL_BUTTON_RIGHT )
        {
          if (editMode)
            engine.decreaseHeight(clickCoords);
          else
            engine.centerScreenOnPoint(clickCoords);
        }
        break;
      case SDL_MOUSEWHEEL:
        if ( event.wheel.y > 0 )
        {
          engine.increaseZoomLevel();
        }
        else if ( event.wheel.y < 0 )
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

    // Render the Frame
    SDL_RenderPresent(_renderer);
    SDL_Delay(1);
  }
  return 0;
}


