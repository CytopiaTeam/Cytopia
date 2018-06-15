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

  _uiElements.push_back(std::make_shared<UiElement> (ButtonImage(x, y, 0)));
  _uiElements.push_back(std::make_shared<UiElement> (ButtonImage(x-22, y-40, 1)));
  _uiElements.push_back(std::make_shared<UiElement> (ButtonImage(x+22, y-40, 2)));
  _uiElements.push_back(std::make_shared<UiElement> (Text(20, 20, "Awesome UI Text!")));
  _uiElements.push_back(std::make_shared<UiElement> (ButtonText(230, 230, 140, 60, 1)));

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
    it->render();
  }
}

bool UIManager::checkClick(int x, int y)
{
  for (auto it : _uiElements)
  {
    if( auto buttonPtr = std::dynamic_pointer_cast<ButtonImage>(it) ) {
      if( buttonPtr->getClickedUiElement(x, y) )
      {
        return true;
      }
    }
  }
  return false;
}

void UIManager::addToGroup(int groupID, std::shared_ptr<UiElement> uiElement)
{
  _group[groupID] = uiElement;
}
