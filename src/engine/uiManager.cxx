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

  _uiElements.push_back(std::make_shared<UiElement> (ButtonImage(x, y, 0, 0, 1, 1)));
  _uiElements.push_back(std::make_shared<UiElement> (ButtonImage(x-22, y-40, 1, 1, 2)));
  _uiElements.push_back(std::make_shared<UiElement> (ButtonImage(x+22, y-40, 2, 1, 3)));
  _uiElements.push_back(std::make_shared<UiElement> (Text(20, 20, "Awesome UI Text!")));
  _uiElements.push_back(std::make_shared<UiElement> (ButtonText(230, 230, 140, 60, 1)));

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