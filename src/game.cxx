#include "game.hxx"


int main(int, char**){

  int screen_height = 800;
  int screen_width = 600;
  float zoom = 1.0;
  bool fullscreen = false;

  Window window("Isometric Engine", screen_height, screen_width);
  Engine engine(window.getSDLRenderer(), window.getSDLWindow());
  Point clickCoords, centerIsoCoords, mouseCoords;
  SDL_Event event;
  

  // Gameloop
  while (!window.isClosed()){
    // Clear the renderer each frame
    SDL_RenderClear(window.getSDLRenderer());
    zoom = engine.getZoomLevel();

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
          case SDLK_f:
            fullscreen = !fullscreen;

            if ( fullscreen )
            {
              SDL_SetWindowFullscreen(window.getSDLWindow(), SDL_WINDOW_FULLSCREEN);
            }
            else
            {
              SDL_SetWindowFullscreen(window.getSDLWindow(), 0);
            }
            break;
        }
      case SDL_MOUSEBUTTONDOWN:
        mouseCoords.setCoords(event.button.x, event.button.y);
        clickCoords = engine.getIsoCoords(mouseCoords);
        if ( event.button.button == SDL_BUTTON_LEFT )
        {
          if ( engine.checkBoundaries(clickCoords) )
          {
            printf("CLICKED - Iso Coords: %d , %d\n", clickCoords.getX(), clickCoords.getY());
            //engine.findNeighbors(clickCoords);
            engine.increaseHeight(clickCoords);
          }
        }
        if ( event.button.button == SDL_BUTTON_RIGHT )
        {
          centerIsoCoords = engine.getIsoCoords(mouseCoords);
          if ( engine.checkBoundaries(centerIsoCoords) )
          {
            engine.centerScreenOnPoint(centerIsoCoords);
          }
        }
        break;
      case SDL_MOUSEWHEEL:
        if ( event.wheel.y > 0 )
        {
          if ( zoom < 2.0 )
          {
            zoom += 0.25;
            engine.setZoomLevel(zoom);
            engine.centerScreenOnPoint(centerIsoCoords);
          }
        }
        else if ( event.wheel.y < 0 )
        {
          if ( zoom > 0.5 )
          {
            zoom -= 0.25;
            engine.setZoomLevel(zoom);
            engine.centerScreenOnPoint(centerIsoCoords);
          }
        }
        break;
      default:
      break;
      }
    }

    // render the tilemap
    engine.render();

    // Render the Frame
    SDL_RenderPresent(window.getSDLRenderer());
  }
  return 0;
}


