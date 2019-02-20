#ifndef BAR_HXX_
#define BAR_HXX_

#include "../basics/uiElement.hxx"

class Bar : public UiElement
{
public:
  Bar(const SDL_Rect &uiElementRect);
  ~Bar() override = default;
};

#endif