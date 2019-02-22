#ifndef BUTTONGROUP_HXX_
#define BUTTONGROUP_HXX_

#include "uiElement.hxx"

class ButtonGroup
{
public:
  ButtonGroup() = default;
  ~ButtonGroup() = default;

  void addToGroup(UiElement *widget);
  void setExclusive(bool exclusive) { exclusive; };
  bool isExclusive() { return exclusive; };

private:
  std::vector<UiElement *> buttonGroup;
  bool exclusive = false;
};

#endif