#ifndef TEXT_HXX_
#define TEXT_HXX_

#include <string>

#include "uiElement.hxx"
#include "../basics/resources.hxx"

class Text : public UiElement
{
public:
  Text() : UiElement() { };
  Text(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) { };
  Text(const SDL_Rect &uiElementRect, const std::string &text);
  Text(const std::string &text);
  ~Text() override = default;
};

#endif