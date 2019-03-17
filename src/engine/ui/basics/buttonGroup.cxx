#include "buttonGroup.hxx"

#include "../../basics/log.hxx"

void ButtonGroup::addToGroup(Button *widget) { m_buttonGroup.push_back(widget); }

bool ButtonGroup::onMouseButtonDown(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    if (it->isVisible())
    {
      if (it->onMouseButtonDown(event))
      {
        return true;
      }
    }
  }

  return false;
}

bool ButtonGroup::onMouseButtonUp(const SDL_Event &event)
{
  for (auto &it : m_buttonGroup)
  {
    if (it->isMouseOver(event.button.x, event.button.y) && it->isVisible())
    {
      if (exclusive)
      {
        uncheckAllButtons();
      }
      else
      {
        uncheckAllButtons(it);
      }
      it->onMouseButtonUp(event);
      return true;
    }
  }

  return false;
}

void ButtonGroup::uncheckAllButtons(Button* exceptThisButton)
{
  for (auto &it : m_buttonGroup)
  {
    if (it == exceptThisButton)
    {
      continue;
    }
    it->setCheckState(false);
  }
}