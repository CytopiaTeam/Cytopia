#include <iostream>
#include "SDL.h"
#include "window.hxx"
#include "sprite.hxx"
#include "map.hxx"
#include "point.hxx"


int main(int, char**){

  int screen_height = 800;
  int screen_width = 600;
  Window window("Isometric Engine", screen_height, screen_width);
  Map map(window.getSDLRenderer(), window.getSDLWindow());
  Point point;

  int cameraoffset_x = 0;
  int cameraoffset_y = 0;
  float zoom = 1.0;
  bool fullscreen = false;


  int pos_x, pos_y;
  float pixel_x, pixel_y;
  int iso_x, iso_y;

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
        
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          
          pos_x = map.getIsoCoordinateX(event.button.x, event.button.y, cameraoffset_x, cameraoffset_y, zoom);
          pos_y = map.getIsoCoordinateY(event.button.x, event.button.y, cameraoffset_x, cameraoffset_y, zoom);
          printf("CLICKED - Iso Coords: %d , %d\n", pos_x, pos_y);
        }
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
          point.x = map.getIsoCoordinateX(event.button.x, event.button.y, cameraoffset_x, cameraoffset_y, zoom);
          point.y = map.getIsoCoordinateY(event.button.x, event.button.y, cameraoffset_x, cameraoffset_y, zoom);
 
          pixel_x = map.getPixelX(point.x, point.y, 0, 0, zoom);
          pixel_y = map.getPixelY(point.x, point.y, 0, 0, zoom);
 
          cameraoffset_x = (pixel_x + (TILE_SIZE*zoom)*0.5) - screen_height * 0.5;
          cameraoffset_y = (pixel_y + (TILE_SIZE*zoom)*0.75) - screen_width * 0.5;
        }
        break;
      case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0)
        {
          if (zoom < 2.0)
          {
            iso_x = map.getIsoCoordinateX(point.x, point.y, cameraoffset_x, cameraoffset_y, zoom);
            iso_y = map.getIsoCoordinateY(point.x, point.y, cameraoffset_x, cameraoffset_y, zoom);

            zoom += 0.25;

            pixel_x = map.getPixelX(point.x, point.y, 0, 0, zoom);
            pixel_y = map.getPixelY(point.x, point.y, 0, 0, zoom);

            cameraoffset_x = (pixel_x + (TILE_SIZE*zoom)*0.5) - screen_width * 0.5;
            cameraoffset_y = (pixel_y + (TILE_SIZE*zoom)*0.75) - screen_height * 0.5;    iso_x = map.getIsoCoordinateX(screen_width / 2, screen_height / 2, cameraoffset_x, cameraoffset_y, zoom);
          }
        }
        else if (event.wheel.y < 0)
        {
          if (zoom > 0.5)
          {
            iso_x = map.getIsoCoordinateX(point.x, point.y, cameraoffset_x, cameraoffset_y, zoom);
            iso_y = map.getIsoCoordinateY(point.x, point.y, cameraoffset_x, cameraoffset_y, zoom);

            //Zooming:
            zoom -= 0.25;

            pixel_x = map.getPixelX(point.x, point.y, 0, 0, zoom);
            pixel_y = map.getPixelY(point.x, point.y, 0, 0, zoom);
            cameraoffset_x = (pixel_x + (TILE_SIZE*zoom)*0.5) - screen_width * 0.5;
            cameraoffset_y = (pixel_y + (TILE_SIZE*zoom)*0.75) - screen_height * 0.5;
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


