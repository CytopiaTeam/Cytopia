#include "buttonGroup.hxx"

#include "../../uiManager.hxx"
#include "../../basics/log.hxx"

void ButtonGroup::addToGroup(Button *widget) { m_buttonGroup.push_back(widget); }

bool ButtonGroup::onMouseButtonDown(const SDL_Event &event)
{
  for (const auto &it : m_buttonGroup)
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
  for (const auto &it : m_buttonGroup)
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

void ButtonGroup::uncheckAllButtons(Button *exceptThisButton)
{
  for (const auto &it : m_buttonGroup)
  {
    // If the buttongroup has children, uncheck them too
    if (it->getUiElementData().actionID == "ToggleVisibilityOfGroup")
    {
      for (const auto &groupElement : UIManager::instance().getUiElementsOfGroup(it->getUiElementData().actionParameter))
      {
        Button *button = dynamic_cast<Button *>(groupElement);
        if (button)
        {
          button->setCheckState(false);
        }
      }
    }
    // skip the calling button itself if it's not exclusive
    if (it == exceptThisButton)
    {
      continue;
    }

    it->setCheckState(false);
  }
}