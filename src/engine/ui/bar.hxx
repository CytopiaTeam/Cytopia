#ifndef BAR_HXX_
#define BAR_HXX_

#include "uiElement.hxx"

class Bar : public UiElement
{
public:
  Bar(int x, int y, int w, int h, int groupID = 0);
  ~Bar() = default;

};

#endif