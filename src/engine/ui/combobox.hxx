#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include "uiElement.hxx"

class ComboBox : public UiElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() = default;

  virtual void draw();

private:
  SDL_Rect rect;

  bool _isPressed = false;
  bool _isMouseOver = false;
};

#endif