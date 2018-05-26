#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <vector>

#include "ui/uiElement.hxx"
#include "ui/button.hxx"

class UIManager
{
public:
  UIManager();
  ~UIManager() = default;

  void init();
  void drawUI();

private:
  std::vector<std::shared_ptr<Button> > _uiButtons;
};

#endif