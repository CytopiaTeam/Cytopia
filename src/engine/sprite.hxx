#ifndef SPRITE_HXX_
#define SPRITE_HXX_


#include <string>
#include <iostream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "basics/point.hxx"
#include "basics/resources.hxx"

const int TILE_SIZE = 32; // All Sprites are 32 x 32

class Sprite
{
public:
  Sprite(std::string filename, Point coords);
  virtual ~Sprite();

  SDL_Texture* _texture;
  SDL_Renderer* _renderer;
  SDL_Window* _window;

private:
  Point _position;
  Point _isoCoords;

  int _heightOffset = 20; // height offset - 20 pixels by trial and error.

  float _zoomLevel;
  Point _cameraOffset;

public:
  void render();

  //Point getTileScreenCoordinates(Point cameraOffset, float zoom = 1.0);
  Point getTileScreenCoordinates();
  Point getTileIsoCoordinates();
  void setTileIsoCoordinates(Point isoCoords);
  void setHeight(int height);
  int getZOrder();

private:
  void loadTexture(std::string file, bool colorkey = false);
  void renderTexture(Point tileScreenCoords, int w, int h);
  void renderTexture(Point tileScreenCoords);

};


#endif 