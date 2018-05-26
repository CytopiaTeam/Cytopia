#include "uiManager.hxx"

UIManager::UIManager()
{

}

void UIManager::init()
{
  _uiButtons.push_back(std::shared_ptr<Button> (new Button(50, 50, 0)));
}

void UIManager::drawUI()
{
  for (auto it : _uiButtons)
  {
    it->render();
  }
}
