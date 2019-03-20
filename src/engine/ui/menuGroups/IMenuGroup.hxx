#include "../basics/uiElement.hxx"
#include <vector>

/**
 * @brief Interface Class for Menu Groups
 * Menu Groups can hold UiElements and format them in a special way. 
 */
class IMenuGroup
{
public:
  /**
   * @brief Pure virtual function for drawing the Menu Group
   * Draw all elements that are within this group. Must be implemented in the derived class.
   */
  virtual void draw () const = 0;

  /**
   * @brief Add an UiElement* to the container.
   * 
   * @param widget Pointer to an UiElement
   */
  void addToGroup(UiElement *widget) { m_groupElements.push_back(widget); };

  /**
   * @brief Visbility of group.
   * If true, the group will not be rendered.
   */
  bool visible = true;

protected:
/**
 * @brief container for storing UiElements
 * 
 */
  std::vector<UiElement *> m_groupElements;

};
