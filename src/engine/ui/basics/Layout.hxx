#ifndef LAYOUT_HXX_
#define LAYOUT_HXX_

#include "../basics/uiElement.hxx"
#include "../basics/buttonGroup.hxx"

#include <map>
#include <vector>
#include <string>
/**
 * @brief MenuGroup for the Build Menu
 * This Menugroup holds all buttons for the BuildMenu
 * Buttons from UiLayout.json are added as "main buttons" for certain categories.
 * All tiles from TileData.json that are in specific categories will be added to this group automatically. The "icon" parameter serves as image for the corresponding ImageButton.
 * All tiles that are not in certain categories will be added to the Debug Menu, which is only shown if Debug Mode has been enabled.
 */
class Layout
{
public:
  /**
 * @brief Construct a new Menu Group Build object
 *
 */
  Layout() = default;
  ~Layout() = default;

  void addToGroup(UiElement *widget)
  {
    if (widget)
    {
      m_groupElements.push_back(widget);
    }
  };

  /**
 * @brief Render all UI Elements
 * 
 */
  void arrangeElements();

private:
  /**
   * @brief Visbility of group.
   * If true, the group will not be rendered.
   */
  bool visible = true;

  std::vector<UiElement *> m_groupElements;

  std::unordered_map<std::string, UiElement *> m_buildSubMenuGroups;
  // This buttongroup holds the main elements
  // TODO: Shared or unique?
  ButtonGroup *m_buildMenuGroup = new ButtonGroup;
};

#endif