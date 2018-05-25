#ifndef BUTTON_HXX_
#define BUTTON_HXX_


#include "uiElement.hxx"

class Button : public UiElement
{
public:
  Button(int x, int y, int uiSpriteID);
  ~Button() = default;

};

#endif