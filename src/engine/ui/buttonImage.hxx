#ifndef BUTTON_HXX_
#define BUTTON_HXX_


#include "uiElement.hxx"

class ButtonImage : public UiElement
{
public:
  ButtonImage(int x, int y, int uiSpriteID, int groupID = 0, int actionID = 0, int parentOfGroup = 0) : UiElement(x, y, uiSpriteID, groupID, actionID, parentOfGroup) {};
  
  //ButtonImage(int x, int y, int uiSpriteID, int groupID, int actionID, int parentOfGroup);
  ~ButtonImage() = default;


};

#endif