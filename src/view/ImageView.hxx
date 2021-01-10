#ifndef IMAGE_VIEW_HXX_
#define IMAGE_VIEW_HXX_

#include <memory>
#include <betterEnums.hxx>

#include "iViewElement.hxx"

/**
 * @class DisplayMode
 * @brief The display mode for an Image
 */
BETTER_ENUM(DisplayMode, uint8_t, RepeatXY, NearestXY);

class ImageView : public iViewElement
{
  std::string m_ImagePath;
  DisplayMode m_DisplayMode;
public:
  ImageView(std::string &&, DisplayMode);
  virtual ~ImageView();
  void draw(iRenderer &) const noexcept override;
  void setup(class GameService & context) noexcept final;
private:
  void nearestXY(class PixelBuffer & pb, Rectangle & target) const noexcept;
  void repeatXY(class PixelBuffer & pb, Rectangle & target) const noexcept;
};

using ImageViewPtr = std::shared_ptr<ImageView>;

#endif // IMAGE_VIEW_HXX_
