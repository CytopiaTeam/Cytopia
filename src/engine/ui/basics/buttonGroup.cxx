#include "buttonGroup.hxx"

#include "../../basics/log.hxx"

void ButtonGroup::addToGroup(Button *widget) { m_buttonGroup.push_back(widget); }

bool ButtonGroup::onMouseButtonDown(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    if (it->onMouseButtonDown(event))
    {
      return true;
    }
  }

  return false;
}

bool ButtonGroup::onMouseButtonUp(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    if (it->onMouseButtonUp(event))
    {
      uncheckAllButtons();
      it->checkState = true;
      return true;
    }
  }

  return false;
}

void ButtonGroup::uncheckAllButtons()
{
  for (auto &it : m_buttonGroup)
  {
    it->checkState = false;
  }
 }