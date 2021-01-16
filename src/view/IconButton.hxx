#ifndef ICON_BUTTON_HXX_
#define ICON_BUTTON_HXX_

#include <memory>
#include "ClassicButton.hxx"

class IconButton final : public ClassicButton<IconButton>
{
public:
  IconButton(const std::string &, const RGBAColor & = Palette::Gray);
  virtual ~IconButton();

  void drawButtonContent(iRenderer &) const noexcept;
  void setupButton(class GameService &context) noexcept;
  void onMouseLeave() noexcept;
  void onDisable() noexcept;
  void onPress() noexcept;
  void onHover() noexcept;

private:
  static constexpr int16_t PADDING_BOTTOM_PRESSED = 9;
  static constexpr int16_t PADDING_BOTTOM_NORMAL = 5;

  std::string m_IconPath;
  int16_t m_IconPaddingBottom;
};

using IconButtonPtr = std::shared_ptr<IconButton>;

#endif // ICON_BUTTON_HXX_
