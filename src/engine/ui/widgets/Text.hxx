#ifndef TEXT_HXX_
#define TEXT_HXX_

#include <string>

#include "../basics/UIElement.hxx"

class Text : public UIElement
{
public:
  Text() = default;
  Text(const SDL_Rect &uiElementRect) : UIElement(uiElementRect){};
  Text(const SDL_Rect &uiElementRect, const std::string &text);
  Text(const std::string &text);
  ~Text() override = default;
};

#endif