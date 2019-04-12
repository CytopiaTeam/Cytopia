#ifndef LAYOUT_HXX_
#define LAYOUT_HXX_

/**
 * @brief Layout functionality
 * Parses the layout parameters from UiLayout and also adds elements to the buildings menu
 * Buttons from UiLayout.json are added as "main buttons" for certain categories.
 * All tiles from TileData.json that are in specific categories will be added to this group automatically. The "icon" parameter serves as image for the corresponding ImageButton.
 * All tiles that are not in certain categories will be added to the Debug Menu, which is only shown if Debug Mode has been enabled.
 */
class Layout
{
public:
  /**
 * @brief Arrange the layout elements
 *
 */
  static void arrangeElements();

private:
  /**
 * @brief Construct a new Menu Group Build object
 *
 */
  Layout() = default;
  /**
 * @brief Destroy the Layout object
 * 
 */
  ~Layout() = default;
};

#endif
