#ifndef TEXT_HXX_
#define TEXT_HXX_

#include <string>

#include "uiElement.hxx"
#include "../basics/resources.hxx"

class Text : public UiElement
{
public:
  Text(const SDL_Rect &uiElementRect);
  Text(const std::string& text);
  ~Text() override = default;
};

#endif