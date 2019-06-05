#ifndef BAR_HXX_
#define BAR_HXX_

#include "../basics/UIElement.hxx"

class Bar : public UIElement
{
public:
  explicit Bar(const SDL_Rect &uiElementRect);
  ~Bar() override = default;
};

#endif