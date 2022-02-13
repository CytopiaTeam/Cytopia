#ifndef CHECKBOX_BUTTON_HXX_
#define CHECKBOX_BUTTON_HXX_

#include <memory>
#include <string>
#include "ClassicButton.hxx"
#include "../model/SettingsModel.hxx"

class CheckBoxButton final : public ClassicButton<CheckBoxButton>, public SettingsModelListener
{
public:
  CheckBoxButton(const bool &, RGBAColor = Palette::Gray);
  virtual ~CheckBoxButton();

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

  static constexpr int PADDING = 8;
  
  int16_t m_TextPaddingBottom;

  // Key for text translation.
  const std::string m_TextKey;
  // Text which will be rendered.
  std::string m_Text;
};

using CheckBoxButtonPtr = std::shared_ptr<CheckBoxButton>;

#endif // TEXT_BUTTON_HXX_
