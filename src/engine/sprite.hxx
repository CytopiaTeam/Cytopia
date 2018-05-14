#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "textureManager.hxx"
#include "basics/point.hxx"
#include "basics/resources.hxx"


class Sprite
{
public:
  Sprite(int tileID, Point coords);
  virtual ~Sprite();

  void render();
  void setTileIsoCoordinates(Point isoCoords);
  void changeTexture(int tileID);

private:
  SDL_Texture* _texture;
  SDL_Renderer* _renderer;
  SDL_Window* _window;

  Point _isoCoordinates;
  Point _screenCoordinates;

  void renderTexture(int w, int h);
  void renderTexture();
};


#endif 