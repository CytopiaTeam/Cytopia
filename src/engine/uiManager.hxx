#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <vector>

#include "ui/button.hxx"
#include "ui/frame.hxx"
#include "ui/text.hxx"
#include "ui/textField.hxx"
#include "ui/tooltip.hxx"
#include "ui/checkbox.hxx"
#include "ui/combobox.hxx"
#include "ui/uiElement.hxx"
#include "basics/resources.hxx"
#include "basics/settings.hxx"
#include "basics/timer.hxx"
#include "textureManager.hxx"
#include "engine.hxx"

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
  std::shared_ptr<Tooltip> getToolTipObject() { return _tooltip; };

  void toggleGroupVisibility(const std::string &groupID);

  void toggleDebugMenu() { _showDebugMenu = !_showDebugMenu; };

  void addToGroup(int groupID, std::shared_ptr<UiElement> uiElement);

  void setButtonState();

  void setFPSCounterText(const std::string &fps);

  const std::vector<std::shared_ptr<UiElement>> &getAllUiElements() const { return _uiElements; };

private:
  UIManager() = default;
  ~UIManager() = default;
  std::vector<std::shared_ptr<UiElement>> _uiElements;
  std::unordered_map<int, std::shared_ptr<UiElement>> _group;

  std::shared_ptr<Tooltip> _tooltip = std::make_shared<Tooltip>();
  // Text element for the FPS Counter (debug menu)
  std::shared_ptr<Text> _fpsCounter = std::make_shared<Text>(SDL_Rect{40, 20, 0, 0});

  bool _mouseHeldDown = false;
  bool _showDebugMenu = false;
};

#endif