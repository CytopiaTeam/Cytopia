#ifndef BUTTONGROUP_HXX_
#define BUTTONGROUP_HXX_

#include "uiElement.hxx"

class ButtonGroup : public UiElement
{
public:
  ButtonGroup() = default;
  ~ButtonGroup() = default;

  void addToGroup(UiElement *widget);
  bool isExclusive() { return exclusive; };

  void onMouseButtonDown(const SDL_Event &event) override;
  void onMouseButtonUp(const SDL_Event &event) override;

  bool exclusive = false;

private:
  std::vector<UiElement *> m_buttonGroup;
};

#endif