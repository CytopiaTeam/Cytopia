#include "buttonGroup.hxx"
#include "../../basics/log.hxx"

void ButtonGroup::addToGroup(UiElement *widget) { m_buttonGroup.push_back(widget); }

void ButtonGroup::onMouseButtonDown(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    it->onMouseButtonDown(event);
  }
}

void ButtonGroup::onMouseButtonUp(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    it->onMouseButtonUp(event);
  }
}