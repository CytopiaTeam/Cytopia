
template <typename ButtonImpl>
ClassicButton<ButtonImpl>::ClassicButton(RGBAColor color) :
  m_Color(color),
  m_At(0)
{ }

template <typename ButtonImpl>
ClassicButton<ButtonImpl>::~ClassicButton() = default;

template <typename ButtonImpl>
void ClassicButton<ButtonImpl>::draw(iRenderer & renderer) const noexcept
{
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
  renderer.drawPicture(target, pb.data());

  /* Next, we draw the content */
  auto impl = static_cast<const ButtonImpl*>(this);
  impl->drawButtonContent(renderer);
}

template <typename ButtonImpl>
void ClassicButton<ButtonImpl>::update(Notification notif) noexcept
{
  auto impl = static_cast<ButtonImpl*>(this);
  auto state = std::get<ButtonStatus>(notif);
  switch(state)
  {
    case ButtonStatus::Normal:
      m_At = 0;
      LOG(LOG_DEBUG) << "Button back to normal";
      impl->onMouseLeave();
      break;
    case ButtonStatus::Disabled:
      m_At = 2;
      LOG(LOG_DEBUG) << "Button disabled";
      impl->onDisable();
      break;
    case ButtonStatus::Pressed:
      m_At = 2;
      LOG(LOG_DEBUG) << "Button pressed";
      impl->onPress();
      break;
    case ButtonStatus::Hovered:
      m_At = 1;
      LOG(LOG_DEBUG) << "Button hovered";
      impl->onHover();
      break;
  }
}

template <typename ButtonImpl>
void ClassicButton<ButtonImpl>::setup(class GameService & context) noexcept
{
  auto impl = static_cast<ButtonImpl*>(this);
  impl->setupButton(context);
}
