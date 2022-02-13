#include "CheckBoxButton.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

CheckBoxButton::CheckBoxButton(const bool &checked, RGBAColor color)
    : m_TextPaddingBottom(PADDING_BOTTOM_NORMAL)
{
}

CheckBoxButton::~CheckBoxButton() = default;

void CheckBoxButton::drawButtonContent(iRenderer &renderer) const noexcept
{
  Rectangle target = getBounds();
  // Rectangle rect;
  target.translateY(m_TextPaddingBottom);
      Rectangle rect{target.getBounds()};
    rect.translateY(target.y1() + 20 + m_TextPaddingBottom);
    rect.translateX(m_TextPaddingBottom);

  // renderer.drawText(m_Text.c_str(), RGBAColor{0xFFFFFFFF}, target, PositionType::Centered);
  renderer.drawShape(rect, RGBAColor{0xFFF33FFF});
}

void CheckBoxButton::setupButton(class GameService &context) noexcept {};

void CheckBoxButton::onMouseLeave() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_NORMAL; }

void CheckBoxButton::onDisable() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_NORMAL; }

void CheckBoxButton::onPress() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_PRESSED; }

void CheckBoxButton::onHover() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_NORMAL; }

void CheckBoxButton::update(SettingsModelListener::Notification notif) noexcept
{
  //auto event = std::get<LanguageChangeEvent>(notif);
  //m_Text = event.localization.translateText(m_TextKey);
}
