#ifndef ASSET_HELPER_HXX_
#define ASSET_HELPER_HXX_

#include <cstdint>
#include <tuple>

#include "Color.hxx"
#include "Shape.hxx"
#include "../util/LOG.hxx"

namespace AssetHelper
{
  void MakeColRepeatImage(uint32_t*, uint32_t*, const Rectangle &, const Rectangle &);

  void GetPixelsFromSurface(SDL_Surface *, uint32_t*);

  /**
   * @brief     Expands a sprite to fit a region
   * @details   Repeats the middle row of pixels in both dimensions
   *            until the surface fits the destination sprite
   * @param     from the original sprite
   * @param     to the destination srpite
   * @param     rfrom the original dimensions
   * @param     rto the destination dimensions
   * @param     corner the dimensions of the corners
   */
  void NNExpandSprite(uint32_t* from, uint32_t* to, 
      const Rectangle & rfrom, const Rectangle & rto, const Rectangle & corner);
  
  /**
   * @brief Shifts the hue of magic pixels
   * @details A magic pixel is one that has an equal red and blue value 
   *          (typically) purple, white, black, or green
   * @see https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB
   */
  size_t CropImage(uint32_t* from, uint32_t* to, const Rectangle & rfrom, Rectangle & rto);

  /**
   * @brief This implementation shifts the hue of magic pixels
   * @details A magic pixel is one that has an equal red and blue value 
   *          (typically) purple, white, black, or green
   * @see https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB
   */
  template <typename Iterator>
  void ColorMagicPixels(Iterator begin, Iterator end, const RGBAColor& color);
  
  void ResizeNearest(uint32_t*, uint32_t*, const Rectangle &, const Rectangle &);

  void Render(uint32_t*, uint32_t*, const Rectangle &, const Rectangle &);

  void RepeatXY(uint32_t*, uint32_t*, const Rectangle &, const Rectangle &);
  
  template <typename OutputIterator>
  static std::pair<int, int> LoadImagePixels(const std::string &, OutputIterator&&);
};

#include "AssetHelper.inl.hxx"
#endif // ASSET_HELPER_HXX_
