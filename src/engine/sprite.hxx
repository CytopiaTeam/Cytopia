#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include <SDL.h>
#include <unordered_map>

#include "basics/point.hxx"
#include "common/Enums.hxx"

class Sprite
{
public:
  Sprite(Point isoCoordinates);
  virtual ~Sprite() = default;

  void render() const;
  void refresh();

  void setTexture(SDL_Texture *m_texture, Layer layer = Layer::TERRAIN);

  size_t spriteCount = 1;
  bool highlightSprite = false;

  SDL_Rect clipRect{0, 0, 0, 0};
  SDL_Rect destRect{0, 0, 0, 0};
  Point isoCoordinates{0, 0, 0, 0};

private:
  std::unordered_map<int, SDL_Texture *> m_textures;
  std::unordered_map<int, SDL_Rect *> m_clipRects;
  SDL_Texture *m_texture = nullptr;
  SDL_Point m_screenCoordinates{0, 0};

  bool m_needsRefresh = false;
  float m_currentZoomLevel = 0;
};

#endif
