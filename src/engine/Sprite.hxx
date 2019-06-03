#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include <SDL.h>

#include "basics/point.hxx"

class Sprite
{
public:
  explicit Sprite(Point isoCoordinates);
  virtual ~Sprite() = default;

  void render() const;
  void refresh();

  void setTexture(SDL_Texture *m_texture);

  size_t spriteCount = 1;
  bool highlightSprite = false;

  SDL_Rect clipRect{0, 0, 0, 0};
  SDL_Rect destRect{0, 0, 0, 0};
  Point isoCoordinates{0, 0, 0, 0};

private:
  SDL_Texture *m_texture = nullptr;
  SDL_Point m_screenCoordinates{0, 0};

  bool m_needsRefresh = false;
  float m_currentZoomLevel = 0;
};

#endif
