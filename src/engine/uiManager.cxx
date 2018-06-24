#include "uiManager.hxx"

void UIManager::init()
{
  nlohmann::json uiLayout = Resources::getUILayoutJSONObject();

  int parentOf = 0;
  int actionID = 0;
  int groupID = 0;
  int x, y;

  for (size_t id = 0; id < uiLayout.size(); id++)
  {
    for (auto it : uiLayout[id].items())
    {

      if (it.key() == "Type")
      {
        if (!uiLayout[id]["GroupID"].is_null())
        {
          groupID = uiLayout[id]["GroupID"].get<int>();
        }

        if (!uiLayout[id]["ParentOfGroup"].is_null())
        {
          parentOf = uiLayout[id]["ParentOfGroup"].get<int>();
        }

        if (!uiLayout[id]["ParentOfGroup"].is_null())
        {
          actionID = uiLayout[id]["ActionID"].get<int>();
        }

        x = uiLayout[id]["Position_x"].get<int>();
        y = uiLayout[id]["Position_y"].get<int>();

        if (it.value() == "ImageButton")
        {
          int spriteID = uiLayout[id]["SpriteID"].get<int>();

          _uiElements.push_back(std::make_shared<Button>(Button(x, y, spriteID, groupID, actionID, parentOf)));
          break;
        }

        else if (it.value() == "TextButton")
        {
          std::string text = uiLayout[id]["Text"].get<std::string>();
          int w = uiLayout[id]["Width"].get<int>();
          int h = uiLayout[id]["Height"].get<int>();

          _uiElements.push_back(std::make_shared<Button>(Button(x, y, w, h, text, groupID, actionID, parentOf)));
          break;
        }

        else if (it.value() == "Frame")
        {
          int w = uiLayout[id]["Width"].get<int>();
          int h = uiLayout[id]["Height"].get<int>();

          _uiElements.push_back(std::make_shared<Frame>(Frame(x, y, w, h, groupID, actionID, parentOf)));
          break;
        }

        else if (it.value() == "Text")
        {
          std::string text = uiLayout[id]["Text"].get<std::string>();

          _uiElements.push_back(std::make_shared<Text>(Text(x, y, text, groupID, actionID, parentOf)));
          break;
        }
        else
        {
          LOG(LOG_ERROR) << "Error in JSON File " << Resources::settings.uiLayoutJSONFile
                         << "\nno matching ui element type for value " << it.value();
        }
      }
    }
  }

  // set all ui elements that are not in the group 0 to invisible.
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    int groupID = it->getGroupID();
    if (groupID != 0)
    {
      _group[groupID] = it;
      it->setVisibility(false);
    }
  }
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
}

std::shared_ptr<UiElement> UIManager::getClickedUIElement(int x, int y)
{
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if (it->getActionID() != 0 && it->isVisible() == true)
      if (it->isClicked(x, y))
      {
        {
          return it;
        }
      }
  }
  return nullptr;
}

void UIManager::addToGroup(int groupID, std::shared_ptr<UiElement> uiElement) { _group[groupID] = uiElement; }

void UIManager::toggleGroupVisibility(int groupID)
{
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if (it->getGroupID() == groupID)
    {
      it->setVisibility(!it->isVisible());
    }
  }
}