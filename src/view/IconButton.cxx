#include "IconButton.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

IconButton::IconButton(const std::string &icon, const RGBAColor &color)
    : m_IconPath(fs::getBasePath() + icon + ".png"), m_Color(color), m_At(0)
{
}

IconButton::~IconButton() {}

void IconButton::draw(iRenderer &renderer) const noexcept
{
  LOG(LOG_DEBUG) << "Drawing a button widget";

  std::string fimage = fs::getBasePath();
  fimage += "resources/images/ui/buttons/button.png";

  /* First, we load the sprite */
  PixelBuffer pb = fs::readPNGFile(fimage);
  Rectangle o_bounds = pb.bounds();
  Rectangle target = getBounds();
  if (o_bounds.width() * o_bounds.height() == 0)
  {
    LOG(LOG_WARNING) << "Could not properly load texture " << fimage;
    return;
  }

  /* Next, we want to display a specific part depending on the state of our button.
   * So we crop that region */
  Rectangle crop_filter{0, 0, o_bounds.width() / 3 - 1, o_bounds.height() - 1};
  pb.crop(std::move(crop_filter));

  /* Next, we transform magic pixels */
  pb.colorMagicPixels(m_Color);

  /* Next, we expand the center of the sprite */
  pb.scale(2.f); // corner radius
  pb.expandCenter(Rectangle{target});

  /* Finally, we draw the picture */
  renderer.drawPicture(target, pb.data());

  /**
   * @todo (vroch): Refactor this code into a "FIT" scale algorithm
   *                in iRenderer
   */
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

void IconButton::update(Notification notif) noexcept
{
  auto state = std::get<ButtonStatus>(notif);
  switch (state)
  {
  case ButtonStatus::Normal:
    m_At = 0;
    LOG(LOG_DEBUG) << "Button back to normal";
    break;
  case ButtonStatus::Disabled:
    m_At = 2;
    LOG(LOG_DEBUG) << "Button disabled";
    break;
  case ButtonStatus::Pressed:
    m_At = 2;
    LOG(LOG_DEBUG) << "Button pressed";
    break;
  case ButtonStatus::Hovered:
    m_At = 1;
    LOG(LOG_DEBUG) << "Button hovered";
    break;
  }
}

void IconButton::setup(class GameService &context) noexcept {};
