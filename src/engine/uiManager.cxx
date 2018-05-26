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

  _uiButtons.push_back(std::shared_ptr<Button> (new Button(x, y, 0)));
}

void UIManager::drawUI()
{
  for (auto it : _uiButtons)
  {
    it->render();
  }
}

bool UIManager::checkClick(int x, int y)
{
  for (auto it : _uiButtons)
  {
    if (it->getClickedUiElement(x, y))
    {
      LOG() << "i am clicked";
      return true;
    }
  }
  return false;
}