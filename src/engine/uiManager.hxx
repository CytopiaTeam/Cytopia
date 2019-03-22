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
#include "ui/menuGroups/MenuGroupBuild.hxx"

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
  void drawUI() const;

  void toggleGroupVisibility(const std::string &groupID, UiElement *sender = nullptr);

  void toggleDebugMenu() { m_showDebugMenu = !m_showDebugMenu; };

  void setFPSCounterText(const std::string &fps);

  const std::vector<std::unique_ptr<UiElement>> &getAllUiElements() const { return m_uiElements; };

  const std::vector<UiElement *> &getUiElementsOfGroup(const std::string groupID) const
  {
    return m_uiGroups.find(groupID)->second;
  };

  UiElement *getUiElementByID(const std::string &UiElement) const;
  void startTooltip(SDL_Event &event, const std::string &tooltipText);

  void stopTooltip() const;

private:
  UIManager() = default;
  ~UIManager() = default;
  // this container holds all UiElements and is the owner.
  std::vector<std::unique_ptr<UiElement>> m_uiElements;

  // subset of UiElements that are not in a group.
  std::vector<UiElement *> m_uiElementsWithoutGroup;

  std::unique_ptr<Tooltip> m_tooltip = std::make_unique<Tooltip>();
  // Text element for the FPS Counter (debug menu)
  std::unique_ptr<Text> m_fpsCounter = std::make_unique<Text>(SDL_Rect{40, 20, 0, 0});

  bool m_showDebugMenu = false;

  std::unordered_map<std::string, std::vector<UiElement *>> m_uiGroups;
  std::unordered_map<std::string, ButtonGroup> m_buttonGroups;

  MenuGroupBuild m_menuGroupBuild;

  friend class EventManager;
};

#endif