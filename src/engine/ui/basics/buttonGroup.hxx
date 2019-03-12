#ifndef BUTTONGROUP_HXX_
#define BUTTONGROUP_HXX_

#include "uiElement.hxx"

class ButtonGroup
{
public:
  ButtonGroup() = default;
  ~ButtonGroup() = default;

  void addToGroup(UiElement *widget);
  bool isExclusive() { return exclusive; };

  bool exclusive = false;

private:
  std::vector<UiElement *> m_buttonGroup;
};

#endif