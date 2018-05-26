#include "button.hxx"

Button::Button(int x, int y, int uiSpriteID) : UiElement(x, y, uiSpriteID)
{

}

bool Button::getClickedUiElement(int x, int y)
{
  if (isClicked(x, y))
  {
    return true;
  }
  return false;
}