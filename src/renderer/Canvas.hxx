#ifndef CANVAS_HXX
#define CANVAS_HXX

#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../renderer/iRenderer.hxx"
#include "../util/Range.hxx"

class Canvas : public iRenderer
{
  TTF_Font * m_Font;
public:
  using BufferData = std::vector<uint32_t>;
  using BufferRange = Range<typename BufferData::iterator>;
  Canvas(const Rectangle &);
  void drawText(const char *, RGBAColor, const Rectangle &, PositionType) final;
  void drawPicture(const Rectangle &, uint32_t*) final;
  void drawShape(const Rectangle &, RGBAColor) final;
  void drawLine() final;
  Rectangle getDrawableSize() const noexcept final;
  BufferRange getBuffer();
  uint32_t* getData();
private:
  void commit() final;
  void clear() final;
  BufferData m_Buffer;
  Rectangle m_Bounds;
};

using CanvasPtr = std::unique_ptr<Canvas>;

#endif // CANVAS_HXX
