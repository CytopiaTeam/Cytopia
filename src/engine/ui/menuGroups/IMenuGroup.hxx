#ifndef I_MENU_GROUP_HXX_
#define I_MENU_GROUP_HXX_

#include "../basics/UIElement.hxx"
#include "../basics/ButtonGroup.hxx"

#include <map>
#include <vector>
/**
 * @brief Interface Class for Menu Groups
 * @details Menu Groups can hold UiElements and format them in a special way. 
 */
class IMenuGroup
{
public:
  /**
   * @brief Pure virtual function for drawing the Menu Group
   * @details Draw all elements that are within this group. Must be implemented in the derived class.
   */
  virtual void draw() const = 0;

  /**
   * @brief Add an UiElement* to the container.
   * @param widget Pointer to a Button
   */
  void addToGroup(Button *widget)
  {
    if (widget)
    {
      m_groupElements.push_back(widget);
    }
  };

  /**
   * @brief Visbility of group.
   * @details If true, the group will not be rendered.
   */
  bool visible = true;

protected:
  /**
   * @brief container for storing UiElements
   */
  std::vector<Button *> m_groupElements;

  std::unordered_map<std::string, ButtonGroup *> m_buildSubMenuGroups;
  // This buttongroup holds the main elements
  // TODO: Shared or unique?
  ButtonGroup *m_buildMenuGroup = new ButtonGroup;

  friend class UIManager;
};

#endif