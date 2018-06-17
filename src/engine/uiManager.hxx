#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <vector>

#include "ui/uiElement.hxx"
#include "ui/text.hxx"
#include "ui/buttonImage.hxx"
#include "ui/buttonText.hxx"
#include "basics/resources.hxx"

class UIManager
{
public:

  /// Retrieves Instance of Singleton class Texture Manager
  static UIManager& Instance() {
    static UIManager instance;
    return instance;
  }

  void init();
  void drawUI();
  std::shared_ptr<UiElement> getClickedUIElement(int x, int y);

  void toggleGroupVisibility(int groupID);
  void enableGroupVisibility(int groupID);
  void disableGroupVisibility(int groupID);
  

  void addToGroup(int groupID, std::shared_ptr<UiElement> uiElement);

private:
  UIManager();
  ~UIManager() = default;
  std::vector<std::shared_ptr<UiElement> > _uiElements;
  /// vector that contains UI Elements of a group
  std::unordered_map<int, std::shared_ptr<UiElement> > _group;
};

#endif