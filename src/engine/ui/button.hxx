#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "uiElement.hxx"

class Button : public UiElement
{
public:
  Button(int x, int y, int uiSpriteID)
      : UiElement(x, y, uiSpriteID){};
  Button(int x, int y, int w, int h, const std::string &text);

  ~Button() = default;

  virtual void draw();

private:
  SDL_Rect rect;

  bool _isPressed = false;
  bool _isMouseOver = false;
};

#endif