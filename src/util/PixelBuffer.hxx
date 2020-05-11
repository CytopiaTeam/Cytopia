#ifndef PIXEL_BUFFER_HXX
#define PIXEL_BUFFER_HXX

#include <vector>
#include "../util/Rectangle.hxx"
#include "../util/Range.hxx"

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
  explicit PixelBuffer(Rectangle &&);
  
  /**
   * @brief   Construct a PixelBuffer from bounds and a range
   * @throws  CytopiaError if 
   *          std::distance(range.begin(), range.end()) != rectange.width() * rectangle.height()
   */
  template <typename Range>
  explicit PixelBuffer(Rectangle &&, Range && range);
  
  using Pixels = Range<std::vector<uint32_t>::const_iterator>;

  /**
   * @brief   Read-only access to pixel data
   */
  Pixels pixels() const;

  /**
   * @brief   Read-only access to bounds
   */
  const Rectangle & bounds() const;

  /**
   * @brief   Scale the pixel buffer using nearest-neighbor 
   *          in both dimensions
   * @param   factor the scaling factor
   * @throws  CytopiaError if factor <= 0
   */
  PixelBuffer & scale(float factor);
  
  /**
   * @brief   Recolors magic pixels
   * @param   color the target color
   * @details A magic pixel is a pixel which satisfies
   *          the condition R == B
   * @post    All magic pixels have the same hue and 
   *          saturation as color, but preserve their lightness
   */
  PixelBuffer & colorMagicPixels(RGBAColor color);


  /**
   * @brief   Constructs a PixelBuffer from a PNG file
   */
  static PixelBuffer fromPNG(std::string);

  /**
   *
   */
  static PixelBuffer 


private:
  Rectangle m_Bounds;
  std::vector<uint32_t> m_Pixels;
};

#endif // PIXEL_BUFFER_HXX
