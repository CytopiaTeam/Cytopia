#ifndef UIMANAGER_HXX_
#define UIMANAGER_HXX_

#include <SDL.h>
#include <vector>

#include "../util/Singleton.hxx"
#include <Layout.hxx>

// BaseClass for game menu, only one menu can show on frame
struct GameMenu
{
  using Ptr = std::shared_ptr<GameMenu>;
  virtual ~GameMenu() = default;
  virtual void draw() const
  { /* default implementation do nothing */
  }
  virtual bool isMouseHovered() const { return false; }
  virtual void closeSubmenus()
  { /* default implementation do nothing */
  }
};

struct BuildMenu;
using json = nlohmann::json;

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

  void loadSettings(json &uiLayout);
  void parseLayouts(const json &uiLayout);

  /**
  * @brief Renders all UI Widgets
  */
  void drawUI();

  /**
 * @brief Toggle Visibility of Debug Menu
 */
  void toggleDebugMenu() { m_showDebugMenu = !m_showDebugMenu; };

  /**
 * @brief Helper function to update the FPS Counter
 * 
 * @param fps 
 */
  void setFPSCounterText(const std::string &fps);

  /**
   * @brief Get the LayoutGroup container
   * @details Returns the container that holds LayoutGroups
   * @return std::unordered_map<std::string, LayoutGroup>& 
   */
  std::unordered_map<std::string, LayoutData> &getLayouts() { return m_layouts; }

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

  template <class Menu> void openMenu() { openMenu(std::make_shared<Menu>()); }

  void closeMenu();
  inline bool isAnyMenuOpen() const { return !m_menuStack.empty(); }

  void addPersistentMenu(GameMenu::Ptr menu);

  bool isMouseHovered() const;

  template <class Menu> inline GameMenu::Ptr findMenu() const
  {
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

  template <class Menu> void addPersistentMenu() { addPersistentMenu(std::make_shared<Menu>()); }

  BUILDMENU_LAYOUT buildMenuLayout() const { return m_buildMenuLayout; }
  void setBuildMenuLayout(BUILDMENU_LAYOUT l) { m_buildMenuLayout = l; }

private:
  UIManager() = default;
  ~UIManager() = default;

  /// map holding layput groups, accessible via the layoutgroup ID
  std::unordered_map<std::string, LayoutData> m_layouts;

  std::string m_tooltip;

  std::unordered_map<std::string, ImFont *> m_loadedFonts;
  std::vector<GameMenu::Ptr> m_menuStack;
  std::vector<GameMenu::Ptr> m_persistentMenu;

  /// visibility of the debug menu
  bool m_showDebugMenu = false;

  std::string m_fpsCounter;
  bool m_showFpsCounter = true;

  /// pointer to the default font used for in-game text
  struct ImFont *fontDefault = nullptr;
  BUILDMENU_LAYOUT m_buildMenuLayout = BUILDMENU_LAYOUT::BOTTOM;
};

#endif
