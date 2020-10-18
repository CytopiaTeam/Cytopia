#ifndef TEXT_BUTTON_HXX_
#define TEXT_BUTTON_HXX_

#include "../model/ButtonState.hxx"
#include "iViewElement.hxx"
#include "../util/Color.hxx"
#include <memory>

class TextButton : public iViewElement, public ButtonListener
{
  int m_At;
  RGBAColor m_Color;
  const char * m_Text;
public:
  TextButton(const char *, RGBAColor = 0x9e9e9eFF);
  virtual void draw(iRenderer &) const noexcept override;
  virtual ~TextButton();
  using MyController = class ButtonHandler;
  using MyState = ButtonState;
  void update(Notification) noexcept final;
  void setup(class GameService & context) noexcept final;
};

using TextButtonPtr = std::shared_ptr<TextButton>;

#endif // TEXT_BUTTON_HXX_
