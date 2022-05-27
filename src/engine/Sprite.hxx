#ifndef SPRITE_HXX_
#define SPRITE_HXX_

#include <SDL.h>
#include <vector>

#include "basics/Point.hxx"
#include "common/enums.hxx"

struct SpriteData
{
  SDL_Texture *texture = nullptr;
  SDL_Rect clipRect{0, 0, 0, 0};
  SDL_Rect destRect{0, 0, 0, 0};
  unsigned char alpha = 255;
};

struct SpriteRGBColor
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

typedef class _sprite_highlight_color_t
{
public:
  static constexpr SpriteRGBColor GRAY{150, 150, 150};
  static constexpr SpriteRGBColor RED{150, 15, 15};
} SpriteHighlightColor;

class Sprite
{
public:
  explicit Sprite(Point isoCoordinates);
  virtual ~Sprite() = default;

  void render() const;
  void refresh(const Layer &layer = Layer::NONE);

  void setTexture(SDL_Texture *m_texture, Layer layer = Layer::TERRAIN);
  void setClipRect(SDL_Rect clipRect, Layer layer = Layer::TERRAIN);
  void setDestRect(SDL_Rect clipRect, Layer layer = Layer::TERRAIN);

  void clearSprite(Layer layer);

  /**
  * @brief enable/disable rendering for this sprite on a specific layer
  * @param layer which layer to set the flag for
  * @param render if this sprite should render this layer
  */
  void setRenderFlag(const Layer &layer, bool render) { m_renderLayer[layer] = render; };

  size_t spriteCount = 1;
  bool highlightSprite = false;
  SpriteRGBColor highlightColor = SpriteHighlightColor::GRAY;

  SDL_Rect getDestRect(Layer layer = Layer::TERRAIN) { return m_SpriteData[layer].destRect; };
  SDL_Rect getClipRect(Layer layer = Layer::TERRAIN) { return m_SpriteData[layer].clipRect; };
  SDL_Rect getActiveClipRect();
  SDL_Rect getActiveDestRect();
  void setSpriteTranparencyFactor(const Layer &layer, unsigned char alpha) { m_SpriteData[layer].alpha = alpha; }
  bool isLayerUsed(Layer layer);

  Point isoCoordinates{0, 0, 0, 0};

private:
  SDL_Point m_screenCoordinates{0, 0};

  bool m_needsRefresh = false;
  double m_currentZoomLevel = 0;

  std::vector<SpriteData> m_SpriteData;
  std::vector<bool> m_renderLayer;
};

#endif
