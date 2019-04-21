#ifndef FRAME_HXX_
#define FRAME_HXX_

#include "../basics/uiElement.hxx"

class Frame : public UiElement
{
public:
  Frame(const SDL_Rect &uiElementRect);
  ~Frame() override = default;

  void draw() override;
  bool isMouseOverHoverableArea(int , int ) override { return false; };
};

#endif