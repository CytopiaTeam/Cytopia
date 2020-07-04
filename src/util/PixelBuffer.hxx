#ifndef PIXEL_BUFFER_HXX
#define PIXEL_BUFFER_HXX

#include <vector>
#include "../util/Rectangle.hxx"
#include "../util/Range.hxx"
#include "../util/Color.hxx"

/**
 * @class     PixelBuffer
 * @brief     A software managed buffer of pixels meant for processing
 */
class PixelBuffer
{
public:
  /**
   * @brief   Constructs an empty PixelBuffer from bounds
   * @post    All pixels are black and fully transparent
   */
  explicit PixelBuffer(const Rectangle &&);
  explicit PixelBuffer(const Rectangle &);

  /**
   * @brief   Construct a PixelBuffer from bounds and a range
   * @throws  CytopiaError if 
   *          std::distance(range.begin(), range.end()) != rectange.width() * rectangle.height()
   */
  template <typename Range> PixelBuffer(const Rectangle &, const Range &range);

  using Pixels = Range<std::vector<uint32_t>::const_iterator>;

  /**
   * @brief   Read-only access to pixel data
   */
  Pixels pixels() const;

  /**
   * @brief   Read-only access to bounds
   */
  const Rectangle &bounds() const;

  /**
   * @brief   Scale the pixel buffer using nearest-neighbor 
   *          in both dimensions
   * @param   factor the scaling factor
   * @post    Becomes EMPTY if factor < 1 / min{width, height}
   */
  PixelBuffer &scale(float factor);

  /**
   * @brief   Recolors magic pixels
   * @param   color the target color
   * @details A magic pixel is a pixel which satisfies
   *          the condition R == B
   * @post    All magic pixels have the same hue as color,
   *          and their lightness and saturation is an overlay
   *          blend of the pixel and color
   */
  PixelBuffer &colorMagicPixels(RGBAColor color);

  bool isEmpty() const noexcept;

  /**
   * @brief   Constructs a PixelBuffer from a PNG file
   */
  static PixelBuffer fromPNG(std::string);

  static PixelBuffer EMPTY();

private:
  Rectangle m_Bounds;
  std::vector<uint32_t> m_Pixels;

  uint8_t overlay255(uint8_t i, uint8_t t);
};

#include "PixelBuffer.inl.hxx"

#endif // PIXEL_BUFFER_HXX
