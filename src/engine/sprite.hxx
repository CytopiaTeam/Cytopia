#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "textureManager.hxx"
#include "basics/point.hxx"
#include "basics/resources.hxx"
#include "basics/settings.hxx"

class Sprite
{
public:
  Sprite(int tileID, Point isoCoordinates);
  virtual ~Sprite() = default;

  void render();
  void setTileIsoCoordinates(Point isoCoords) { _isoCoordinates = isoCoords; };
  void changeTexture(int tileID) { _texture = TextureManager::Instance().getTileTexture(tileID); };
  SDL_Rect getTextureInformation() { return _destRect; };

private:
  SDL_Texture *_texture;
  SDL_Renderer *_renderer;
  SDL_Window *_window;
  SDL_Rect _destRect;

  Point _isoCoordinates;
  Point _screenCoordinates;
  int _tileID;

  /** Renders texture using the _destRect struct and takes the zoomLevel into account */
  void renderTexture(int w, int h);
  /** Renders texture using the textures original width and height without zoomLevel */
  void renderTexture();
};

#endif