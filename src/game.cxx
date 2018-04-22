#include <iostream>
#include "SDL.h"
#include "window.hxx"
#include "sprite.hxx"
#include "map.hxx"


int main(int, char**){

  Window window("Isometric Engine", 800, 600);
  Map map(window.getSDLRenderer(), window.getSDLWindow());


  int cameraoffset_x = -20;
  int cameraoffset_y = -20;
  float zoom = 1.0;
  bool fullscreen = false;

  // Gameloop
  while (!window.isClosed()){
    // Clear the renderer each frame
    SDL_RenderClear(window.getSDLRenderer());




    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        window.close();
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
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
        }
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


