#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include "SDL2/SDL.h"

#include "basics/point.hxx"

class Sprite
{
public:
  Sprite(int tileID, Point isoCoordinates);
  virtual ~Sprite() = default;

  void render();
  void updateCoordinates();

  void changeTexture(int tileID);
  void setTileIsoCoordinates(Point isoCoords) { _isoCoordinates = isoCoords; }

  const SDL_Rect &getTextureInformation() { return _destRect; };

private:
  SDL_Texture *_texture;
  SDL_Renderer *_renderer;
  SDL_Window *_window;
  SDL_Rect _destRect;

  Point _isoCoordinates;
  Point _screenCoordinates;
  int _tileID;
  float _zoomLevel;
  int _tileSize;
  int offscreenTolerance;
};

#endif