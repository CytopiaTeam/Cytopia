#include "IconButton.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

IconButton::IconButton(const std::string &icon, const RGBAColor &color) : 
  ClassicButton(color),
  m_IconPath(icon)
{
}

IconButton::~IconButton() = default;

void IconButton::onMouseLeave() noexcept { }

void IconButton::onDisable() noexcept { }

void IconButton::onPress() noexcept { }

void IconButton::onHover() noexcept { }

void IconButton::drawButtonContent(iRenderer &renderer) const noexcept
{
  /**
   * @todo (vroch): Refactor this code into a "FIT" scale algorithm
   *                in iRenderer
   */
  auto target = getBounds();
  auto icon = fs::readPNGFile(m_IconPath);
  const auto &iconBounds = icon.bounds();
  float scaleX = target.width() - 6;
  scaleX /= iconBounds.width();
  float scaleY = target.height() - 15; // 7 pixels to account for the bevel effect
  scaleY /= iconBounds.height();
  float iconScale = std::min(scaleX, scaleY);
  icon.scale(iconScale);
  auto [x, y] = target.p1();
  icon.translateX(x + (target.width() - iconBounds.width()) / 2);
  icon.translateY(y + 4);
  renderer.drawPicture(icon.bounds(), icon.data());
}

void IconButton::setupButton(class GameService &context) noexcept {};
