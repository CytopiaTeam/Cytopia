#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include "uiElement.hxx"
#include "textField.hxx"

class ComboBox : public UiElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() override = default;

  void draw() override;

private:
  SDL_Rect rect;
  SDL_Rect menuRect;


  std::shared_ptr<TextField> _textField;

  bool _isPressed = false;
  bool _isMouseOver = false;
};

#endif