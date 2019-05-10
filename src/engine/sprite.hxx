#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include <SDL.h>
#include <unordered_map>

#include "basics/point.hxx"
#include "common/enums.hxx"

struct SpriteData
{
  SDL_Texture *texture = nullptr;
  SDL_Rect clipRect{0, 0, 0, 0};
  SDL_Rect destRect{0, 0, 0, 0};
};

class Sprite
{
public:
  Sprite(Point isoCoordinates);
  virtual ~Sprite() = default;

  void render() const;
  void refresh();

  void setTexture(SDL_Texture *m_texture, Layer layer = Layer::TERRAIN);
  void setClipRect(SDL_Rect clipRect, Layer layer = Layer::TERRAIN);
  void setDestRect(SDL_Rect clipRect, Layer layer = Layer::TERRAIN);

  size_t spriteCount = 1;
  bool highlightSprite = false;

  SDL_Rect getDestRect(Layer layer = Layer::TERRAIN) { return m_SpriteData[layer].destRect; };
  SDL_Rect getActiveClipRect();

  Point isoCoordinates{0, 0, 0, 0};

private:
  SDL_Texture *m_texture = nullptr;
  SDL_Point m_screenCoordinates{0, 0};

  bool m_needsRefresh = false;
  float m_currentZoomLevel = 0;

  std::vector<SpriteData> m_SpriteData;
};

#endif
