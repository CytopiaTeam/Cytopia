#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <SDL.h>
#include <vector>

#include "ui/basics/UIElement.hxx"
#include "ui/basics/Layout.hxx"
#include "ui/widgets/Text.hxx"
#include "../util/Singleton.hxx"

/**
 * @brief Struct that hold UiElements belonging to a layoutgroup and its corresponding LayoutData
 * 
 */
struct LayoutGroup
{
  std::vector<UIElement *> uiElements; ///< contains pointer to all uiElements belonging to this group
  LayoutData layout;                   ///< layout information @see LayoutData
};

// BaseClass for game menu, only one menu can show on frame
struct GameMenu
{
  using Ptr = std::shared_ptr<GameMenu>;
  virtual ~GameMenu() = default;
  virtual void draw() const { /* default implementation do nothing */ }
  virtual bool isMouseHovered() const { return false; }
  virtual void closeSubmenus() { /* default implementation do nothing */ }
};

struct BuildMenu;

enum class BUILDMENU_LAYOUT
{
  LEFT = 0,
  RIGHT = 1,
  TOP = 2,
  BOTTOM = 3
};

/**
 * @brief Draws the UI to the screen
 * @details Parses UiLayout.json file and instantiates UI widgets accordingly. Also takes care of layouting
 */
class UIManager : public Singleton<UIManager>
{
public:
  friend Singleton<UIManager>;

  /**
   * @brief Parses the UiLayout.json files and creates UI Elements
   * 
   */

  void init();

  struct ImFont *loadFont(const std::string &name, uint32_t size);
  void initializeImGuiFonts();

  void loadSettings(json& uiLayout);
  void parseLayouts(const json &uiLayout);

  /**
  * @brief Renders all UI Widgets
  */
  void drawUI() const;

  void setGroupVisibility(const std::string &groupID, bool visible);
  /**
 * @brief Callback function for toggling the visibility of an UiGroup 
 * @details Callback function for Ui Widgets with the ActionID "ToggleVisiblityOfGroup".
 * The ActionParamter is used for toggling elements from m_uiGroups with the same ID
 * 
 * @param groupID The groupID that should be toggled
 * @param sender The element who called the function
 */
  void toggleGroupVisibility(const std::string &groupID, UIElement *sender = nullptr);

  /**
 * @brief Toggle Visibility of Debug Menu
 */
  void toggleDebugMenu() { m_showDebugMenu = !m_showDebugMenu; };

  /**
 * @brief Helper function to update the FPS Counter
 * 
 * @param fps 
 */
  void setFPSCounterText(const std::string &fps) const;

  /**
 * @brief Get all Ui Element objects
 * Returns a container that holds ALL UiElements.
 * @return const std::vector<std::unique_ptr<UiElement>>& 
 */
  const std::vector<std::unique_ptr<UIElement>> &getAllUiElements() const { return m_uiElements; };

  /**
 * @brief Get all Ui Elements For Event Handling 
 * @details Returns Widgets that are not in ButtonGroups and ButtonGroups.
 * @return const std::vector<UiElement *>& 
 */
  const std::vector<UIElement *> &getAllUiElementsForEventHandling() const { return m_uiElementsForEventHandling; };

  /**
 * @brief Get the UiElements Of aroup 
 * @details get Elements that are in UiGroup (m_uiGroups)
 * @param groupID ID of the group whose elements should be returned
 * @return const std::vector<UiElement *>* 
 */
  const std::vector<UIElement *> *getUiElementsOfGroup(const std::string &groupID) const;

  /**
 * @brief Get the m_uiGroups object
 * @details Returns the container that holds all UiElements sorted by their UI Group
 * @return std::unordered_map<std::string, std::vector<UiElement *>>& 
 */
  std::unordered_map<std::string, std::vector<UIElement *>> &getAllUiGroups() { return m_uiGroups; }

  /**
   * @brief Get the LayoutGroup container
   * @details Returns the container that holds LayoutGroups
   * @return std::unordered_map<std::string, LayoutGroup>& 
   */
  std::unordered_map<std::string, LayoutGroup> &getAllLayoutGroups() { return m_layoutGroups; }

  /**
 * @brief Get the Ui Element By ID
 * @details Finds and returns an UiElement by its elementID parameter
 * @param ID ID of the element that should be returned.
 * @return UIElement* 
 */
  UIElement *getUiElementByID(const std::string &ID) const;


  void setTooltip(const std::string &tooltipText);
  
  /**
   * @brief Hides and resets the active tooltip.
   * @see Tooltip#reset
   */
  void clearTooltip();

  /**
 * @brief Close all open menus but the build menu
 */
  void closeOpenMenus();

  void openMenu(GameMenu::Ptr menuOption);

  template<class Menu, class ... Args>
  void openMenuA(Args&&...args) { openMenu(std::make_shared<Menu>(args...)); }

  template<class Menu>
  void openMenu() { openMenu(std::make_shared<Menu>()); }

  void closeMenu();
  inline bool isAnyMenuOpen() const { return !m_menuStack.empty(); }

  void addPersistentMenu(GameMenu::Ptr menu);

  bool isMouseHovered() const;

  template<class Menu>
  inline GameMenu::Ptr findMenu() const {
    for (const auto &m : m_menuStack)
    {
      if (dynamic_cast<Menu *>(m.get()))
      {
        return m;
      }
    }

    for (const auto &m : m_persistentMenu)
    {
      if (dynamic_cast<Menu *>(m.get()))
      {
        return m;
      }
    }

    return nullptr;
  }
  
  template<class Menu>
  void addPersistentMenu() { addPersistentMenu(std::make_shared<Menu>()); }

  BUILDMENU_LAYOUT buildMenuLayout() const { return m_buildMenuLayout; }
  void setBuildMenuLayout(BUILDMENU_LAYOUT l) { m_buildMenuLayout = l; }

private:

  UIManager() = default;
  ~UIManager() = default;

  /// this container holds all UiElements and is the owner.
  std::vector<std::unique_ptr<UIElement>> m_uiElements;

  /// stores  UiElements that are not in a group
  std::vector<UIElement *> m_uiElementsForEventHandling;

  /// map holding all ui elements, accessible via the group ID
  std::unordered_map<std::string, std::vector<UIElement *>> m_uiGroups;

  /// map holding layput groups, accessible via the layoutgroup ID
  std::unordered_map<std::string, LayoutGroup> m_layoutGroups;

  std::string m_tooltip;

  /// Text element for the FPS Counter (debug menu)
  std::unique_ptr<Text> m_fpsCounter = std::make_unique<Text>();

  std::unordered_map<std::string, ImFont *> m_loadedFonts;
  std::vector<GameMenu::Ptr> m_menuStack;
  std::vector<GameMenu::Ptr> m_persistentMenu;

  void addToLayoutGroup(const std::string &groupName, UIElement *widget);

  /// visibility of the debug menu
  bool m_showDebugMenu = false;

  /// pointer to the default font used for in-game text
  struct ImFont *fontDefault = nullptr;
  BUILDMENU_LAYOUT m_buildMenuLayout = BUILDMENU_LAYOUT::BOTTOM;
};

#endif
