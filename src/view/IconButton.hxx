#ifndef ICON_BUTTON_HXX_
#define ICON_BUTTON_HXX_

#include <memory>
#include "ClassicButton.hxx"

class IconButton : public ClassicButton<IconButton>
{
public:
  IconButton(const std::string &, const RGBAColor& = Palette::Gray);
  virtual ~IconButton();
  
  void drawButtonContent(iRenderer &) const noexcept;
  void setupButton(class GameService & context) noexcept;
  void onMouseLeave() noexcept;
  void onDisable() noexcept;
  void onPress() noexcept;
  void onHover() noexcept;
private:
  std::string m_IconPath;
};

using IconButtonPtr = std::shared_ptr<IconButton>;

#endif // ICON_BUTTON_HXX_
