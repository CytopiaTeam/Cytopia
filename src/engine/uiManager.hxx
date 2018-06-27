#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <vector>

#include "ui/uiElement.hxx"
#include "ui/text.hxx"
#include "ui/frame.hxx"
#include "ui/button.hxx"
#include "basics/resources.hxx"
#include "textureManager.hxx"

#include "../ThirdParty/json.hxx"

class UIManager
{
public:
  /// Retrieves Instance of Singleton class Texture Manager
  static UIManager &Instance()
  {
    static UIManager instance;
    return instance;
  }

  void init();
  void drawUI();
  std::shared_ptr<UiElement> getClickedUIElement(int x, int y);

  void toggleGroupVisibility(int groupID);

  void addToGroup(int groupID, std::shared_ptr<UiElement> uiElement);

  void setButtonState();

private:
  UIManager() = default;
  ~UIManager() = default;
  std::vector<std::shared_ptr<UiElement>> _uiElements;
  std::unordered_map<int, std::shared_ptr<UiElement>> _group;
};

#endif