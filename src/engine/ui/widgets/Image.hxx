#ifndef IMAGE_HXX_
#define IMAGE_HXX_

#include <string>

#include "../basics/UIElement.hxx"

/**
 * @brief An Image GUI Widget
 */
class Image : public UIElement
{
public:
  Image() = default;
  ~Image() override = default;

  void draw() override;

  /** @brief sets the opacity of the image
   * @param alphaLevel value from 0 to 255 that corresponds to image's opacity where 0 is transparent and 255 is opaque
   */
  void setOpacity(Uint8 alphaLevel) { m_alphaLevel = alphaLevel; };

private:
  /// the alpha value of the image
  Uint8 m_alphaLevel = 255;
};

#endif