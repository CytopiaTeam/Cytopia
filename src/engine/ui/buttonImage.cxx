#include "buttonImage.hxx"

ButtonImage::ButtonImage(int x, int y, int uiSpriteID, int actionID) : UiElement(x, y, uiSpriteID)
{
  setAction(actionID);
}

