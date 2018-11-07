#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include "SDL2/SDL.h"

#include "basics/point.hxx"

class Sprite
{
public:
  Sprite(SDL_Texture *texture, Point isoCoordinates);
  virtual ~Sprite() = default;

  void render();
  void refresh();

  void setTexture(SDL_Texture *texture);
  void setCoordinates(Point isoCoords) { _isoCoordinates = isoCoords; }

  const SDL_Rect &getTextureInformation() { return _destRect; };

private:
  SDL_Texture *_texture = nullptr;
  SDL_Renderer *_renderer;
  SDL_Window *_window;
  SDL_Rect _destRect;

  Point _isoCoordinates;
  Point _screenCoordinates;

  float _zoomLevel;
  int _tileSize;
};

#endif