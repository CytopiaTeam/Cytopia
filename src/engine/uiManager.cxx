#include "uiManager.hxx"

UIManager::UIManager()
{

}

void UIManager::init()
{
  // This class should load the whole ui layout from an json file.
  
  // Bottom buttons
  // middle of the screen - half of the imagesize to center it
  int x = Resources::settings.screenWidth / 2 - 32;
  // y is the bottom of the screen - imagesize - offset
  int y = Resources::settings.screenHeight - 64 - 16;
  //std::shared_ptr<Button> button_construct = std::shared_ptr<Button>(new Button(x, y, 0));

  nlohmann::json uiLayout = Resources::getUILayoutJSONObject();

  int parentOf = 0;
  int actionID = 0;
  int groupID = 0;

  for (size_t id = 0; id < uiLayout.size() ; id++)
  {
    for (auto it : json::iterator_wrapper(uiLayout[id]))
    {

      if ( it.key() == "Type" )
      {
        if ( ! uiLayout[id]["GroupID"].is_null() )
        {
          groupID = uiLayout[id]["GroupID"].get<int>();
        }

        if ( ! uiLayout[id]["ParentOfGroup"].is_null() )
        {
          parentOf = uiLayout[id]["ParentOfGroup"].get<int>();
        }

        if ( ! uiLayout[id]["ParentOfGroup"].is_null() )
        {
          actionID = uiLayout[id]["ActionID"].get<int>();
        }

        if ( it.value() == "ImageButton" )
        {
          int spriteID = uiLayout[id]["SpriteID"].get<int>() ;
          int x = uiLayout[id]["Position_x"].get<int>();
          int y = uiLayout[id]["Position_y"].get<int>();
          _uiElements.push_back(std::make_shared<UiElement>(ButtonImage(x, y, spriteID, groupID, actionID, parentOf)));
          break;
        }

        else if ( it.value() == "TextButton" )
        {
          std::string text = uiLayout[id]["Text"].get<std::string>();
          int x = uiLayout[id]["Position_x"].get<int>();
          int y = uiLayout[id]["Position_y"].get<int>();
          int w = uiLayout[id]["Width"].get<int>();
          int h = uiLayout[id]["Height"].get<int>();

         _uiElements.push_back(std::make_shared<UiElement> (ButtonText(x, y, w, h, text, groupID, actionID, parentOf)));
          break;
        }
        else if (it.value() == "Text")
        {
          std::string text = uiLayout[id]["Text"].get<std::string>();
          int x = uiLayout[id]["Position_x"].get<int>();
          int y = uiLayout[id]["Position_y"].get<int>();

          _uiElements.push_back(std::make_shared<UiElement> (Text(x, y, text, groupID, actionID, parentOf)));
          break;
        }
        else
        {
          LOG(LOG_ERROR) << "Error in JSON File " << Resources::settings.uiLayoutJSONFile << "\nno matching ui type for value " << it.value();
        }
      }
    }
  }

  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    int groupID = it->getGroupID();
    if ( groupID != 0 )
    {
      _group[groupID] = it;
      it->setVisibility(false);
    }
  }

}

//Point UIManager::calculatePosition

void UIManager::drawUI()
{
  for (auto it : _uiElements)
  {
    if (it->isVisible())
    {
      it->render();
    }
  }
}

std::shared_ptr<UiElement> UIManager::getClickedUIElement(int x, int y)
{
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if( it->getActionID() != 0 && it->isVisible() == true) 
    {
      if( it->isClicked(x, y) )
      {
        return it;
      }
    }
  }
  return 0;
}



void UIManager::addToGroup(int groupID, std::shared_ptr<UiElement> uiElement)
{
  _group[groupID] = uiElement;
}

void UIManager::toggleGroupVisibility(int groupID)
{
  for (std::shared_ptr<UiElement> it : _uiElements)
  {
    if ( it->getGroupID() == groupID )
    {
      it->setVisibility( ! it->isVisible() );
    }
  }
}