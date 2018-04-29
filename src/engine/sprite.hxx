#ifndef SPRITE_HXX_
#define SPRITE_HXX_


#include <string>
#include <iostream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "texture.hxx"
#include "basics/point.hxx"

const int TILE_SIZE = 32; // All Sprites are 32 x 32

class Sprite
{
public:
  Sprite(std::string filename, Point coords, SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Sprite();

  SDL_Texture *_texture;
  SDL_Renderer *_renderer;
  SDL_Window *_window;

private:
  Point _position;
  Point _isoCoords;

  int _heightOffset = 19; // height offset - 19 pixels by trial and error.

public:
  void render(Point cameraOffset, float zoom = 1.0);

  Point getTileScreenCoordinates(Point cameraOffset, float zoom = 1.0);
  Point getTileIsoCoordinates();
  void setTileIsoCoordinates(Point isoCoords);
  int getZOrder();
};


#endif 