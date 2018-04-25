#include <iostream>
#include "SDL.h"
#include "window.hxx"
#include "sprite.hxx"
#include "map.hxx"
#include "point.hxx"


int main(int, char**){

  int screen_height = 800;
  int screen_width = 600;
  int cameraoffset_x = 0;
  int cameraoffset_y = 0;
  float zoom = 1.0;
  bool fullscreen = false;

  Window window("Isometric Engine", screen_height, screen_width);
  Map map(window.getSDLRenderer(), window.getSDLWindow());
  Point clickCoords, centerScreenCoords, mouseCoords;
  SDL_Event event;
  

  // Gameloop
  while (!window.isClosed()){
    // Clear the renderer each frame
    SDL_RenderClear(window.getSDLRenderer());

    if (SDL_PollEvent(&event))
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
          case SDLK_g:
            map.toggleGrid();
            printf("Toggle Grid\n");
            break;
          case SDLK_LEFT:
            cameraoffset_x += 4;
            break;
          case SDLK_RIGHT:
            cameraoffset_x -= 4;
            break;
          case SDLK_UP:
            cameraoffset_y += 4;
            break;
          case SDLK_DOWN:
            cameraoffset_y -= 4;
            break;
          case SDLK_f:
            fullscreen = !fullscreen;

            if (fullscreen)
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
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          clickCoords = map.getIsoCoords(mouseCoords, cameraoffset_x, cameraoffset_y, zoom);
          printf("CLICKED - Iso Coords: %d , %d\n", clickCoords.x, clickCoords.y);
        }
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
          clickCoords = map.getIsoCoords(mouseCoords, cameraoffset_x, cameraoffset_y, zoom);
          centerScreenCoords = map.getScreenCoords(clickCoords, 0, 0, zoom);
 
          cameraoffset_x = (centerScreenCoords.x + (TILE_SIZE*zoom)*0.5) - screen_height * 0.5;
          cameraoffset_y = (centerScreenCoords.y + (TILE_SIZE*zoom)*0.75) - screen_width * 0.5;
        }
        break;
      case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0)
        {
          if (zoom < 2.0)
          {
            // TODO: Fix zooming bug
            //point2 = map.getScreenCoords(centerCoords, 0, 0, zoom);
            //centerScreenCoords = map.getScreenCoords(map.getIsoCoords(centerScreenCoords, cameraoffset_x, cameraoffset_y, zoom), 0, 0, zoom);

            zoom += 0.25;

            cameraoffset_x = (centerScreenCoords.x + (TILE_SIZE*zoom)*0.5) - screen_width * 0.5;
            cameraoffset_y = (centerScreenCoords.y + (TILE_SIZE*zoom)*0.75) - screen_height * 0.5;
          }
        }
        else if (event.wheel.y < 0)
        {
          if (zoom > 0.5)
          {
            zoom -= 0.25;

            cameraoffset_x = (centerScreenCoords.x + (TILE_SIZE*zoom)*0.5) - screen_width * 0.5;
            cameraoffset_y = (centerScreenCoords.y + (TILE_SIZE*zoom)*0.75) - screen_height * 0.5;
          }
        }
        break;
      default:
      break;
      }
    }

    // render the tilemap
    map.render(cameraoffset_x, cameraoffset_y, zoom);

    // Render the Frame
    SDL_RenderPresent(window.getSDLRenderer());
  }
  return 0;
}


