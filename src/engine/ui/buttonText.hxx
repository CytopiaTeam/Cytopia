#ifndef BUTTONTEXT_HXX_
#define BUTTONTEXT_HXX_


#include "uiElement.hxx"

class ButtonText : public UiElement
{
public:
  ButtonText(int x, int y, int w, int h, int groupID = 0, int actionID = 0, int parentOfGroup = 0);

  ~ButtonText() = default;

  void addLabel(const std::string & text);

};

#endif