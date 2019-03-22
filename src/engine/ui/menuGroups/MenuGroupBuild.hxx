#ifndef MENU_GROUP_BUILD_HXX_
#define MENU_GROUP_BUILD_HXX_

#include "IMenuGroup.hxx"

/**
 * @brief MenuGroup for the Build Menu
 * This Menugroup holds all buttons for the BuildMenu
 * Buttons from UiLayout.json are added as "main buttons" for certain categories.
 * All tiles from TileData.json that are in specific categories will be added to this group automatically. The "icon" parameter serves as image for the corresponding ImageButton.
 * All tiles that are not in certain categories will be added to the Debug Menu, which is only shown if Debug Mode has been enabled.
 */
class MenuGroupBuild : public IMenuGroup
{
public:
  /**
 * @brief Construct a new Menu Group Build object
 * 
 */
  MenuGroupBuild() = default;

  /**
 * @brief Render all UI Elements
 * 
 */
  void draw() const override;

  void constructMenu();
};

#endif