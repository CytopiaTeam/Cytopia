#ifndef BUTTONTEXT_HXX_
#define BUTTONTEXT_HXX_


#include "uiElement.hxx"

class ButtonText : public UiElement
{
public:
  ButtonText(int x, int y, int w, int h, const std::string& text, int groupID = 0, int actionID = 0, int parentOfGroup = 0);
  ~ButtonText() = default;

};

#endif