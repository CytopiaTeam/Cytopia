#include "game.hxx"


int main(int, char**)
{
  LOG() << VERSION;
  LOG().timerStart();
  Resources::init();

  WindowManager window("Isometric Engine");
  Engine& engine = Engine::Instance();
  LOG().timerEnd();
  LOG() << "Tile Matrix initialized";

  Point clickCoords, mouseCoords;
  SDL_Event event;
  EventManager evManager = EventManager();

  UIManager& uiManager = UIManager::Instance();
  uiManager.init();
  
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
          
          case SDLK_w:
            // just for debug 
            Resources::generateINIFile();
            break;

          case SDLK_j:
            // just for debug 
            Resources::generateUITextureFile();
            break;
          
          case SDLK_f:
            window.toggleFullScreen();
            break;
        }
        break;
 
      case SDL_MOUSEBUTTONDOWN:
        // hacky event handling for now
        if (evManager.checkEvents(event))
        {
          // events are handled -> done
          break;
        }
        

        mouseCoords.setCoords(event.button.x, event.button.y);
        clickCoords = Resources::convertScreenToIsoCoordinates(mouseCoords);
        if ( event.button.button == SDL_BUTTON_LEFT )
        {
          if ( engine.checkBoundaries(clickCoords) )
          {
            if (Resources::getEditMode())
              engine.increaseHeight(clickCoords);
            else
              LOG() << "CLICKED - Iso Coords: " << clickCoords.getX() << ", " << clickCoords.getY();
          }
        }
        else if ( event.button.button == SDL_BUTTON_RIGHT )
        {
          if (Resources::getEditMode())
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

    uiManager.drawUI();

    // Render the Frame
    SDL_RenderPresent(_renderer);
    SDL_Delay(1);
  }
  return 0;
}


