#ifndef TEXT_BUTTON_HXX_
#define TEXT_BUTTON_HXX_

#include <memory>
#include <string>
#include "ClassicButton.hxx"
#include "../model/SettingsModel.hxx"

class TextButton final : public ClassicButton<TextButton>, public SettingsModelListener
{
public:
  TextButton(const std::string &, const std::string &, RGBAColor = Palette::Gray);
  virtual ~TextButton();

  void drawButtonContent(iRenderer &) const noexcept;
  void setupButton(class GameService &context) noexcept;
  void onMouseLeave() noexcept;
  void onDisable() noexcept;
  void onPress() noexcept;
  void onHover() noexcept;
  void update(SettingsModelListener::Notification) noexcept final;

private:
  static constexpr int16_t PADDING_BOTTOM_PRESSED = 2;
  static constexpr int16_t PADDING_BOTTOM_NORMAL = 0;

  int16_t m_TextPaddingBottom;

  // Key for text translation.
  const std::string m_TextKey;
  // Text which will be rendered.
  std::string m_Text;
};

using TextButtonPtr = std::shared_ptr<TextButton>;

#endif // TEXT_BUTTON_HXX_
