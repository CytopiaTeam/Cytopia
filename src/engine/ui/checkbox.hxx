#ifndef CHECKBOX_HXX_
#define CHECKBOX_HXX_

#include "uiElement.hxx"

class Checkbox : public UiElement
{
public:
  Checkbox(const SDL_Rect &uiElementRect);
  ~Checkbox() override = default;

  SDL_Rect rect;
  void draw() override;

private:
  bool checked = false;
};

#endif