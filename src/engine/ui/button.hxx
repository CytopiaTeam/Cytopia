#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "uiElement.hxx"

class Button : public UiElement
{
public:
  Button(const SDL_Rect &uiElementRect);
  ~Button() override = default;

  void draw() override;

private:
  SDL_Rect rect;

  bool _isPressed = false;
  bool _isMouseOver = false;
};

#endif