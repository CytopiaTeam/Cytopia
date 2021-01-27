#include "TextView.hxx"

TextView::TextView(const std::string &keyText, const std::string &text, RGBAColor color)
    : m_TextKey(keyText), m_Text(text), m_Color(color)
{
}

TextView::~TextView() = default;

void TextView::draw(iRenderer &renderer) const noexcept
{
  renderer.drawText(m_Text.c_str(), m_Color, getBounds(), PositionType::Centered);
}

void TextView::setup(class GameService &context) noexcept {}

void TextView::update(SettingsModelListener::Notification notif) noexcept
{
  auto event = std::get<LanguageChangeEvent>(notif);
  m_Text = event.localization.translateText(m_TextKey);
}