#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <vector>

#include "ui/uiElement.hxx"
#include "ui/text.hxx"
#include "ui/button.hxx"
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
  bool checkClick(int x, int y);

private:
  UIManager();
  ~UIManager() = default;
  std::vector<std::shared_ptr<UiElement> > _uiElements;
};

#endif