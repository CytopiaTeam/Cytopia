#ifndef PIXEL_BUFFER_HXX
#define PIXEL_BUFFER_HXX

#include <vector>
#include "../util/Rectangle.hxx"
#include "../util/Range.hxx"
#include "../util/Color.hxx"

/**
 * @class     PixelBuffer
 * @brief     A software managed buffer of pixels meant for processing
 * @details   PixelBuffer is used to process pixels displayed on the screen in software.
 *            It is the pairing of a buffer of pixels with a Rectangle region describing
 *            where these pixels are mapped.
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
  template <typename Range>
  PixelBuffer(const Rectangle &, const Range & range);
  
  using Pixels = Range<std::vector<uint32_t>::const_iterator>;

  /**
   * @brief   Read-only access to pixel data
   * @details Each pixel is defined by an unsigned 32 bit integer following the
   *          RGBA format with channel depth of 1 byte. In total, there are exactly
   *          bounds().width() * bounds.height() pixels
   */
  Pixels pixels() const;

  /**
   * @brief   Read-only access to the bounds of the PixelBuffer
   * @details Boundaries are the rectangle coordinates of the buffer 
   *          describing their position, width, and height
   */
  const Rectangle & bounds() const;

  /**
   * @brief   Scale the pixel buffer using nearest-neighbor 
   *          in both dimensions
   * @param   factor the scaling factor
   * @post    Becomes EMPTY if factor < 1 / min{width, height}
   */
  PixelBuffer & scale(float factor);
  
  /**
   * @brief   Crop the pixel buffer around a region
   * @param   region the region to crop to
   * @details The cropped region is the intersection of region and bounds
   */
  PixelBuffer & crop(Rectangle && region);

  /**
   * @brief   Recolors magic pixels
   * @param   color the target color
   * @details A magic pixel is a pixel which satisfies
   *          the condition R == B
   * @post    All magic pixels have the same hue as color,
   *          and their lightness and saturation is an overlay
   *          blend of the pixel and color
   */
  PixelBuffer & colorMagicPixels(RGBAColor color);

  /**
   * @brief   Expands the sprite with a center cross to reach a dimension
   * @param   target the target region to expand over
   */
  PixelBuffer & expandCenter(Rectangle && target);

  bool isEmpty() const noexcept;

  uint32_t * data() noexcept;
  
  static PixelBuffer EMPTY();

  /**
   * @brief   Convert to SDL_Surface
   * @details The lifetime of the returned surface must not exceed
   *          the lifetime of the PixelBuffer
   */
  SDL_Surface* toSurface() noexcept;

private:
  Rectangle m_Bounds;
  std::vector<uint32_t> m_Pixels;

  float overlay(float i, float t);
};

#include "PixelBuffer.inl.hxx"

#endif // PIXEL_BUFFER_HXX
