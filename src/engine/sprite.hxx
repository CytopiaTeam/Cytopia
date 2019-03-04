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

  void setTexture(SDL_Texture *m_texture);

  size_t spriceCount = 1;
  bool highlightSprite = false;

  SDL_Rect clipRect{0, 0, 0, 0};
  SDL_Rect destRect;
  Point isoCoordinates;

private:
  SDL_Texture *m_texture = nullptr;
  SDL_Point m_screenCoordinates;

  bool m_needsRefresh = false;
  float m_currentZoomLevel = 0;
};

#endif
