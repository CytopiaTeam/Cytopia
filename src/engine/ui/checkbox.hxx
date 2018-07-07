#ifndef CHECKBOX_HXX_
#define CHECKBOX_HXX_

#include "uiElement.hxx"

class Checkbox : public UiElement
{
public:
  Checkbox(const SDL_Rect &uiElementRect);
  ~Checkbox() = default;

  SDL_Rect rect;
  virtual void draw();

private:
  bool checked = false;
};

#endif