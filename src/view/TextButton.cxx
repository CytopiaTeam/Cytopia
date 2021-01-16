#include "TextButton.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

TextButton::TextButton(const std::string &keyText, const std::string &text, RGBAColor color)
    : m_TextPaddingBottom(PADDING_BOTTOM_NORMAL), m_TextKey(keyText), m_Text(text)
{
}

TextButton::~TextButton() = default;

void TextButton::drawButtonContent(iRenderer &renderer) const noexcept
{
  Rectangle target = getBounds();
  target.translateY(m_TextPaddingBottom);
  renderer.drawText(m_Text.c_str(), RGBAColor{0xFFFFFFFF}, target, PositionType::Centered);
}

void TextButton::setupButton(class GameService &context) noexcept {};

void TextButton::onMouseLeave() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_NORMAL; }

void TextButton::onDisable() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_NORMAL; }

void TextButton::onPress() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_PRESSED; }

void TextButton::onHover() noexcept { m_TextPaddingBottom = PADDING_BOTTOM_NORMAL; }

void TextButton::update(SettingsModelListener::Notification notif) noexcept
{
  auto event = std::get<LanguageChangeEvent>(notif);
  m_Text = event.localization.translateText(m_TextKey);
}
