#include <iostream>
#include "SDL.h"
#include "window.hxx"
#include "sprite.hxx"
#include "map.hxx"


int main(int, char**){

  Window window("Isometric Engine", 800, 600);
  Map map(window.getSDLRenderer(), window.getSDLWindow());

  while (!window.isClosed()){

    int cameraoffset_x = -20;
    int cameraoffset_y = -20;
    float zoom = 1;

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
        printf("BUTTON PRESSED\n");
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          window.close();
          break;
        case SDLK_g:
          map.toggleGrid();
          printf("GSPRESSED\n");
          break;
        case SDLK_LEFT:
          printf("LEFT BUTTON PRESSED\n");
          break;
        }
      default:
        break;
      }
    }


    map.render(cameraoffset_x, cameraoffset_y, zoom);

    // Render the Frame
    SDL_RenderPresent(window.getSDLRenderer());

  }
  return 0;
  
  
}


