#include "uiManager.hxx"

void UIManager::init()
{
  nlohmann::json uiLayout = Resources::getUILayoutJSONObject();

  for (const auto &it : uiLayout.items())
  {
    std::string groupID;
    groupID = it.key();

    bool visible = false;

    for (size_t id = 0; id < uiLayout[it.key()].size(); id++)
    {

      if (!uiLayout[it.key()][id]["groupVisibility"].is_null())
      {
        visible = uiLayout[it.key()][id]["groupVisibility"].get<bool>();
      }

      if (!uiLayout[it.key()][id]["Type"].is_null())
      {
        int actionID = 0;
        std::string parentOf;
        std::string tooltipText;
        std::string text;

        SDL_Rect elementRect = {0, 0, 0, 0};
        int spriteID = -1;

        // Each element must have x and y values
        elementRect.x = uiLayout[it.key()][id]["Position_x"].get<int>();
        elementRect.y = uiLayout[it.key()][id]["Position_y"].get<int>();

        if (!uiLayout[it.key()][id]["Width"].is_null())
        {
          elementRect.w = uiLayout[it.key()][id]["Width"].get<int>();
        }
        if (!uiLayout[it.key()][id]["Height"].is_null())
        {
          elementRect.h = uiLayout[it.key()][id]["Height"].get<int>();
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

        std::shared_ptr<UiElement> uiElement;
        // Create the ui elements
        if (uiLayout[it.key()][id]["Type"] == "ImageButton")
        {
          uiElement = std::make_shared<Button>(Button(elementRect));
          uiElement->setSpriteID(spriteID);
        }
        if (uiLayout[it.key()][id]["Type"] == "TextButton")
        {
          uiElement = std::make_shared<Button>(Button(elementRect));
          uiElement->setText(text);
        }
        if (uiLayout[it.key()][id]["Type"] == "Text")
        {
          uiElement = std::make_shared<Text>(Text(elementRect));
          uiElement->setText(text);
        }
        if (uiLayout[it.key()][id]["Type"] == "Frame")
        {
          uiElement = std::make_shared<Frame>(Frame(elementRect));
        }
        if (uiLayout[it.key()][id]["Type"] == "Checkbox")
        {
          uiElement = std::make_shared<Checkbox>(Checkbox(elementRect));
        }
        if (uiLayout[it.key()][id]["Type"] == "ComboBox")
        {
          uiElement = std::make_shared<ComboBox>(ComboBox(elementRect));
          uiElement->setText(text);
        }

        uiElement->setVisibility(visible);
        uiElement->setTooltipText(tooltipText);
        uiElement->setActionID(actionID);
        uiElement->setParentID(parentOf);
        uiElement->setGroupID(groupID);

        // store the element in a vector
        _uiElements.emplace_back(uiElement);
      }
    }
  }

  _tooltip->setVisibility(false);
}

void UIManager::setFPSCounterText(const std::string &fps) { _fpsCounter->setText(fps); }

void UIManager::drawUI()
{
  for (const auto &it : _uiElements)
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
  if (_showDebugMenu)
  {
    _fpsCounter->draw();
  }
}

void UIManager::setButtonState()
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  for (const std::shared_ptr<UiElement> &it : _uiElements)
  {
    if (it->isClicked(x, y))
    {

      if (it->isToogleButton())
      {
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT) && !_mouseHeldDown)
        {
          if (it->getButtonState() == TextureManager::TOGGLED)
          {
            it->changeButtonState(TextureManager::DEFAULT);
          }
          else
          {
            it->changeButtonState(TextureManager::TOGGLED);
          }

          _mouseHeldDown = true;
        }
        else if (!SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
          _mouseHeldDown = false;
        }
      }
      else
      {
        if (it->isVisible())
        {
          if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT))
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

  for (const std::shared_ptr<UiElement> &it : _uiElements)
  {
    if (it->isClicked(x, y))
    {
      if (it->getActionID() != -1 && it->isVisible())
        clickedElement = it;
    }
  }
  return clickedElement;
}

void UIManager::addToGroup(int groupID, std::shared_ptr<UiElement> uiElement) { _group[groupID] = std::move(uiElement); }

void UIManager::toggleGroupVisibility(const std::string &groupID)
{
  for (const std::shared_ptr<UiElement> &it : _uiElements)
  {
    if (it->getGroupID() == groupID)
    {
      it->setVisibility(!it->isVisible());
    }
  }
}
