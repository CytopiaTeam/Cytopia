#ifndef FRAME_HXX_
#define FRAME_HXX_

#include "uiElement.hxx"

class Frame : public UiElement
{
public:
  Frame(const SDL_Rect &uiElementRect);
  ~Frame() override = default;

  void draw() override;
};

#endif