#ifndef TEXT_BUTTON_HXX_
#define TEXT_BUTTON_HXX_

#include <memory>
#include <string>
#include "ClassicButton.hxx"

class TextButton : public ClassicButton<TextButton>
{
public:
  TextButton(const std::string &, RGBAColor = Palette::Gray);
  virtual ~TextButton();
  
  void drawButtonContent(iRenderer &) const noexcept;
  void setupButton(class GameService & context) noexcept;
  void onMouseLeave() noexcept;
  void onDisable() noexcept;
  void onPress() noexcept;
  void onHover() noexcept;
private:
  static constexpr int16_t PADDING_BOTTOM_PRESSED = -10;
  static constexpr int16_t PADDING_BOTTOM_DISABLED = -10;
  static constexpr int16_t PADDING_BOTTOM_HOVERED = 5;
  
  int16_t m_TextPaddingBottom;
  std::string m_Text;
};

using TextButtonPtr = std::shared_ptr<TextButton>;

#endif // TEXT_BUTTON_HXX_
