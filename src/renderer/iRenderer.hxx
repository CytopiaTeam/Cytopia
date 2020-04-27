#ifndef I_RENDERER_HXX_
#define I_RENDERER_HXX_

#include <memory>
#include <tuple>
#include <betterEnums.hxx>
#include "../util/Color.hxx"

class Rectangle;
BETTER_ENUM(PositionType, uint8_t, Centered, Normal);

class iRenderer
{
public:
  virtual ~iRenderer() = 0;
  virtual void drawText(const char *, RGBAColor, const Rectangle &, PositionType) = 0;
  virtual void drawPicture(const Rectangle &, uint32_t *) = 0;
  virtual void drawShape(const Rectangle &, RGBAColor) = 0;
  virtual void drawLine() = 0;
  virtual Rectangle getDrawableSize() const noexcept = 0;

private:
  virtual void commit() = 0;
  virtual void clear() = 0;
  friend class Window;
};

using iRendererPtr = std::unique_ptr<iRenderer>;

#endif // I_RENDERER_HXX_
