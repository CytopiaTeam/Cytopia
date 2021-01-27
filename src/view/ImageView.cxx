#include "ImageView.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"
#include <vector>

ImageView::ImageView(std::string && image, DisplayMode mode) : 
  m_ImagePath(fs::getBasePath() + image + ".png"),
  m_DisplayMode(mode)
{ }

ImageView::~ImageView() { }

void ImageView::draw(iRenderer & renderer) const noexcept
{
  Rectangle target = getBounds();
  PixelBuffer pb = fs::readPNGFile(m_ImagePath);
  switch(m_DisplayMode)
  {
    case DisplayMode::NearestXY:
      nearestXY(pb, target);
      break;
    case DisplayMode::RepeatXY:
      repeatXY(pb, target);
  }
  if(pb.isEmpty())
  {
    return;
  }
  renderer.drawPicture(target, pb.data());
}

void ImageView::nearestXY(PixelBuffer & pb, Rectangle & target) const noexcept
{
  // Preserve aspect ratio
  Rectangle o_bounds = pb.bounds();
  float width_factor = o_bounds.width();
  width_factor = target.width() / width_factor;
  float height_factor = o_bounds.height();
  height_factor = target.height() / height_factor;
  float factor = std::min(width_factor, height_factor);
  pb.scale(factor);
  Rectangle nearest = pb.bounds();
  // Center nearest within target
  nearest.translateX((target.width() - nearest.width()) / 2 + target.x1());
  nearest.translateY((target.height() - nearest.height()) / 2 + target.y1());
  std::swap(target, nearest);
}

void ImageView::repeatXY(PixelBuffer & pb, Rectangle & target) const noexcept
{
  /**
   * @todo Implement this
   */
  LOG(LOG_WARNING) << TRACE_INFO "RepeatXY Mode for ImageView not implemented";
  pb = PixelBuffer::EMPTY();
  target = Rectangle::EMPTY();
}

void ImageView::setup(class GameService & context) noexcept { };
