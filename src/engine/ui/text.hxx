#include <string>

#include "uiElement.hxx"
#include "../basics/resources.hxx"

class Text : public UiElement
{
public:
  Text(const SDL_Rect &uiElementRect);
  ~Text() = default;
};