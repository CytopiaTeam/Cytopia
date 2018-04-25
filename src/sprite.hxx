#ifndef SPRITE_HXX_
#define SPRITE_HXX_


#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "texture.hxx"
#include "point.hxx";

const int TILE_SIZE = 32; // All Sprites are 32 x 32

class Sprite
{
public:
  Sprite(std::string filename, int x, int y, SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Sprite();

  SDL_Texture *_texture;
  SDL_Renderer *_renderer;
  SDL_Window *_window;

private:
  Point _position;
  Point _isoCoords;

public:
  void render(int cameraoffset_x, int cameraoffset_y, float zoom = 1.0, int height = 0.);
  int getZOrder();

  Point getTileScreenCoordinates(int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);

  
  Point getTileIsoCoordinates() const
  {
    return _isoCoords;
  }

  void setTileIsoCoordinates(int posX, int posY)
  {
    _isoCoords.x = posX;
    _isoCoords.y = posY;
  }
};


#endif 