#include "TextButton.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

TextButton::TextButton(const char * text, RGBAColor color) :
  m_At(0), 
  m_Color(color),
  m_Text(text)
{
}

TextButton::~TextButton() = default;

void TextButton::draw(iRenderer & renderer) const noexcept
{
  LOG(LOG_DEBUG) << "Drawing a button widget";
  std::string fimage = fs::getBasePath();
  fimage += "resources/images/ui/buttons/button.png";

  /* First, we load the sprite */
  PixelBuffer pb = fs::readPNGFile(fimage);
  Rectangle o_bounds = pb.bounds();
  Rectangle target = getBounds();
  if(o_bounds.width() * o_bounds.height() == 0)
  {
    LOG(LOG_WARNING) << "Could not properly load texture " << fimage;
    return;
  }

  /* Next, we want to display a specific part depending on the state of our button.
   * So we crop that region */
  Rectangle crop_filter {0, 0, o_bounds.width() / 3 - 1, o_bounds.height() - 1};
  pb.crop(std::move(crop_filter));

  /* Next, we transform magic pixels */
  pb.colorMagicPixels(m_Color);

  /* Next, we expand the center of the sprite */
  pb.scale(2.f);
  pb.expandCenter(Rectangle{target});
  
  /* Finally, we draw the picture & text */
  renderer.drawPicture(target, pb.data());
  /*if(m_At == 2)
  {
    rect.translateY(5);
  }*/
  renderer.drawText(m_Text, RGBAColor{0xFFFFFFFF}, target, PositionType::Centered);
}

void TextButton::update(Notification notif) noexcept
{
  auto state = std::get<ButtonStatus>(notif);
  switch(state)
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

void TextButton::setup(class GameService & context) noexcept { };
