#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <vector>

#include "ui/widgets/button.hxx"
#include "ui/widgets/frame.hxx"
#include "ui/widgets/text.hxx"
#include "ui/widgets/textField.hxx"
#include "ui/widgets/tooltip.hxx"
#include "ui/widgets/checkbox.hxx"
#include "ui/widgets/combobox.hxx"
#include "ui/basics/uiElement.hxx"
#include "ui/basics/buttonGroup.hxx"

#include <SDL.h>

class UIManager
{
public:
  /// Retrieves instance of Singleton class UI Manager
  static UIManager &instance()
  {
    static UIManager uiManager;
    return uiManager;
  }
  void init();
  void drawUI();

  void toggleGroupVisibility(const std::string &groupID);

  void toggleDebugMenu() { m_showDebugMenu = !m_showDebugMenu; };

  void setFPSCounterText(const std::string &fps);

  const std::vector<std::unique_ptr<UiElement>> &getAllUiElements() const { return m_uiElements; };

  UiElement *getUiElementByID(const std::string &UiElement);
  void startTooltip(SDL_Event &event, const std::string &tooltipText);

  void stopTooltip();

private:
  UIManager() = default;
  ~UIManager() = default;
  std::vector<std::unique_ptr<UiElement>> m_uiElements;

  std::unique_ptr<Tooltip> m_tooltip = std::make_unique<Tooltip>();
  // Text element for the FPS Counter (debug menu)
  std::unique_ptr<Text> m_fpsCounter = std::make_unique<Text>(SDL_Rect{40, 20, 0, 0});

  bool m_showDebugMenu = false;

  std::unordered_map<std::string, std::vector<UiElement *>> m_uiGroups;
  std::unordered_map<std::string, ButtonGroup> m_buttonGroups;
};

#endif