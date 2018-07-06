#include "uiManager.hxx"

void UIManager::init()
{
  nlohmann::json uiLayout = Resources::getUILayoutJSONObject();
  Timer UITimer;

  for (auto it : uiLayout.items())
  {
    std::string groupID;

    groupID = it.key();
    for (size_t id = 0; id < uiLayout[it.key()].size(); id++)
    {

      int actionID = 0;
      std::string parentOf;
      std::string tooltipText = "";
      std::string text = "";
      int x = 0;
      int y = 0;
      int w = 0;
      int h = 0;
      int spriteID = 0;

      // Each element must have x and y values
      x = uiLayout[it.key()][id]["Position_x"].get<int>();
      y = uiLayout[it.key()][id]["Position_y"].get<int>();

      if (!uiLayout[it.key()][id]["Width"].is_null())
      {
        w = uiLayout[it.key()][id]["Width"].get<int>();
      }
      if (!uiLayout[it.key()][id]["Height"].is_null())
      {
        h = uiLayout[it.key()][id]["Height"].get<int>();
      }
      if (!uiLayout[it.key()][id]["SpriteID"].is_null())
      {
        spriteID = uiLayout[it.key()][id]["SpriteID"].get<int>();
      }
      if (!uiLayout[it.key()][id]["TooltipText"].is_null())
      {
        tooltipText = uiLayout[it.key()][id]["TooltipText"].get<std::string>();
      }
      if (!uiLayout[it.key()][id]["ParentOfGroup"].is_null())
      {
        parentOf = uiLayout[it.key()][id]["ParentOfGroup"].get<std::string>();
      }
      if (!uiLayout[it.key()][id]["ActionID"].is_null())
      {
        actionID = uiLayout[it.key()][id]["ActionID"].get<int>();
      }
      if (!uiLayout[it.key()][id]["Text"].is_null())
      {
        text = uiLayout[it.key()][id]["Text"].get<std::string>();
      }

      // Create the ui elements
      if (uiLayout[it.key()][id]["Type"] == "ImageButton")
      {
        _uiElements.push_back(std::make_shared<Button>(Button(x, y, spriteID, groupID, actionID, parentOf, tooltipText)));
      }
      if (uiLayout[it.key()][id]["Type"] == "TextButton")
      {
        _uiElements.push_back(std::make_shared<Button>(Button(x, y, w, h, text, groupID, actionID, parentOf, tooltipText)));
      }
      if (uiLayout[it.key()][id]["Type"] == "Text")
      {
      }
      if (uiLayout[it.key()][id]["Type"] == "Frame")
      {
        _uiElements.push_back(std::make_shared<Frame>(Frame(x, y, w, h, groupID, actionID, parentOf, tooltipText)));
      }
    }
  }

  //// set all ui elements that are not in the group 0 to invisible.
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if (!(it->getGroupID() == "BottomBar"))
    {
      //_group[groupID] = it;
      it->setVisibility(false);
    }
  }

  _tooltip->setVisibility(false);
}

void UIManager::drawUI()
{
  for (auto it : _uiElements)
  {
    if (it->isVisible())
    {
      it->draw();
    }
  }
  if (_tooltip->isVisible())
  {
    _tooltip->draw();
  }
}

void UIManager::setButtonState()
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if (it->isClicked(x, y))
    {

      if (it->isToogleButton())
      {
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && _mouseHeldDown == false)
        {
          if (it->getButtonState() == TextureManager::TOGGLED)
            it->changeButtonState(TextureManager::DEFAULT);
          else
            it->changeButtonState(TextureManager::TOGGLED);

          _mouseHeldDown = true;
        }
        else if (!SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
          _mouseHeldDown = false;
        }
      }
      else
      {
        if (it->isVisible())
        {
          if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
          {
            it->changeButtonState(TextureManager::CLICKED);
          }
          else
          {
            it->changeButtonState(TextureManager::HOVERING);
          }
        }
      }
    }
    else
    {
      if (!it->isToogleButton())
      {
        it->changeButtonState(TextureManager::DEFAULT);
      }
    }
  }
}

std::shared_ptr<UiElement> UIManager::getClickedUIElement(int x, int y)
{
  std::shared_ptr<UiElement> clickedElement = nullptr;

  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if (it->isClicked(x, y))
    {
      if (it->getActionID() != -1 && it->isVisible() == true)
        clickedElement = it;
    }
  }
  return clickedElement;
}

void UIManager::addToGroup(int groupID, std::shared_ptr<UiElement> uiElement) { _group[groupID] = uiElement; }

void UIManager::toggleGroupVisibility(const std::string &groupID)
{
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if (it->getGroupID() == groupID)
    {
      it->setVisibility(!it->isVisible());
    }
  }
}
