#ifndef CLASSIC_BUTTON_HXX_
#define CLASSIC_BUTTON_HXX_

#include "../model/ButtonModel.hxx"
#include "iViewElement.hxx"
#include "../util/Color.hxx"
#include "../util/Filesystem.hxx"
#include "../renderer/iRenderer.hxx"

template <typename ButtonImpl> class ClassicButton : public iViewElement, public ButtonListener
{
public:
  using MyController = class ButtonHandler;
  using MyState = ButtonModel;
  explicit ClassicButton(RGBAColor = Palette::Gray);
  void draw(iRenderer &) const noexcept override;
  virtual ~ClassicButton();
  void update(Notification) noexcept final;
  void setup(GameService &context) noexcept override;

private:
  static constexpr const char *BUTTON_SPRITE_PATH = "resources/images/ui/buttons/button.png";
  RGBAColor m_Color;
  uint16_t m_At;
};

#include "ClassicButton.inl.hxx"

#endif // CLASSIC_BUTTON_HXX_
