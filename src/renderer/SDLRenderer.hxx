#ifndef SDL_RENDERER_HXX_
#define SDL_RENDERER_HXX_
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>

#include "iRenderer.hxx"

class SDLRenderer final : public iRenderer
{
  SDL_Renderer * m_Renderer = nullptr;
  TTF_Font * m_Font = nullptr;
public:
  SDLRenderer(SDL_Window *);
  ~SDLRenderer() final;
  void drawText(const char *, RGBAColor, const Rectangle &, PositionType) final;
  void drawPicture(const Rectangle &, uint32_t*) final;
  void drawShape(const Rectangle &, RGBAColor) final;
  void drawLine() final;
  Rectangle getDrawableSize() const noexcept final;
  void setViewport(const Rectangle &) final;
  void resetViewport() final;
  /**
   * @todo Remove this when new ui is complete
   */
  friend class WindowManager;
private:
  void commit() final;
  void clear() final;
};

#endif // SDL_RENDERER_HXX_
