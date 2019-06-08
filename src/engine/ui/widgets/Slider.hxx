#ifndef SLIDER_HXX_
#define SLIDER_HXX_

#include <string>

#include "../basics/UIElement.hxx"

class Slider : public UIElement
{
public:
  Slider(const SDL_Rect &uiElementRect);

  Slider() = default;
  ~Slider() override = default;

  void draw() override;


private:
};

#endif