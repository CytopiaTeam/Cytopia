#ifndef TEXT_HXX_
#define TEXT_HXX_

#include <string>

#include "uiElement.hxx"

class Text : public UiElement
{
public:
  Text() = default;
  Text(const SDL_Rect &uiElementRect) : UiElement(uiElementRect){};
  Text(const SDL_Rect &uiElementRect, const std::string &text);
  Text(const std::string &text);
  ~Text() override = default;
};

#endif