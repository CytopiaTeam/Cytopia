#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include <SDL.h>

#include "basics/point.hxx"

class Sprite
{
public:
  Sprite(Point isoCoordinates);
  virtual ~Sprite() = default;

  void render();
  void refresh();

  void setClipRect(const SDL_Rect &clipRect) { _clipRect = clipRect; };

  void setTexture(SDL_Texture *texture);
  void setCoordinates(Point isoCoords) { _isoCoordinates = isoCoords; }
  void setSpriteCount(size_t spriteCount) { _spriteCount = spriteCount; };
  const SDL_Rect &getDestRect() { return _destRect; };
  const SDL_Rect &getClipRect() { return _clipRect; };

  void highlight(bool highlight) { _highlightSprite = highlight; };

private:
  SDL_Texture *_texture = nullptr;
  SDL_Rect _destRect;
  SDL_Rect _clipRect{0, 0, 0, 0};

  Point _isoCoordinates;
  SDL_Point _screenCoordinates;

  float _currentZoomLevel = 0;
  size_t _spriteCount = 1;

  bool _needsRefresh = false;
  bool _highlightSprite = false;
};

#endif
