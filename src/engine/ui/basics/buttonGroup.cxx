#include "buttonGroup.hxx"
#include "../../basics/log.hxx"

void ButtonGroup::addToGroup(UiElement *widget) { m_buttonGroup.push_back(widget); }

bool ButtonGroup::onMouseButtonDown(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    it->onMouseButtonDown(event);
  // should return true if return value is true
  }

  return false;
}

bool ButtonGroup::onMouseButtonUp(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    it->onMouseButtonUp(event);
  // should return true if return value is true
  }

  return false;
}