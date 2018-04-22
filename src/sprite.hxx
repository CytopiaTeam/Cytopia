#ifndef SPRITE_HXX_
#define SPRITE_HXX_


#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "texture.hxx"

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
  int _pos_x;
  int _pos_y;

  bool _moving;
  int _dest_x;
  int _dest_y;

public:
  void render(int cameraoffset_x, int cameraoffset_y, float zoom = 1.0, int height = 0.);

  float getPixelX(int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);
  float getPixelY(int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);

  float getZOrder();

  int getPosX() const
  {
    return _pos_x;
  }

  void setPosX(int posx)
  {
    _pos_x = posx;
  }

  int getPosY() const
  {
    return _pos_y;
  }

  void setPosY(int posy)
  {
    _pos_y = posy;
  }

  int getDestX() const
  {
    return _dest_x;
  }

  void setDestX(int destx)
  {
    _dest_x = destx;
  }

  int getDestY() const
  {
    return _dest_y;
  }

  void setDestY(int desty)
  {
    _dest_y = desty;
  }

  bool isMoving() const
  {
    return _moving;
  }

  void setMoving(bool moving)
  {
    _moving = moving;
  }

};


#endif 