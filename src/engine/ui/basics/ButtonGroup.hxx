#ifndef BUTTONGROUP_HXX_
#define BUTTONGROUP_HXX_

#include "UIElement.hxx"
#include "../widgets/Button.hxx"

/**
 * @brief Class to group buttons and to make sure there's only one active toggle button in the group
 * When adding buttons to the group, only one item can be active at the same time. If another button is clicked while another one is active, the last one will be un-clicked (unchecked)
 * @note Only toggle buttons should be added to a ButtonGroup.
 */
class ButtonGroup : public UIElement
{
public:
  /**
   * @brief Construct a new Button Group object
   */
  ButtonGroup() = default;
  /**
   * @brief Destroy the Button Group object
   */
  ~ButtonGroup() = default;

  /**
   * @brief Add a (toggle-)Button object to the button group
   * Add a Button to the group. Only toggleable buttons should be added.
   * @param widget The Button that should be added
   */
  void addToGroup(Button *widget);

  bool onMouseButtonDown(const SDL_Event &event) override;
  bool onMouseButtonUp(const SDL_Event &event) override;

  /// The number of buttons in the group
  size_t count() { return m_buttonGroup.size(); };

  /**
   * @brief Defines wheter on button of the group must stay active or not
   * If enabled, one button always stays active. Keep in mind that the button group should be initialized with one button set to checked.
   * Best suitable for Comboboxes or Radio Buttons.
   * @note this only works if the ButtonGroup is set to exlusive! Also take in mind, that this option doesn't make sense if the group has children (ToggleVisibilityOfGroup)
   */
  bool alwaysOn = false;

  /**
  * @brief Defines whether the button group is exclusive
  * If set to true, only one button in the group can be checked at the same time.
  */
  bool exclusive = true;

  /**
   * @brief Gets all of the buttons in this ButtonGroup
   * @returns a vector of pointers to the buttons in this ButtonGroup
   */
  const std::vector<Button *> getAllButtons() const { return m_buttonGroup; };

private:
  /// The buttons in the group
  std::vector<Button *> m_buttonGroup;

  /**
   * @brief Function to call the uncheck event of buttons.
   * Function to call the uncheck event of buttons.
   * Keep in mind that this also triggers the buttons signals. Signals for toggle buttons should be aware of their checkstate and use it accordingly.
   * @param exceptThisButton Does not toggle this button. Usually the caller, if exclusive is enabled.
   */
  void uncheckAllButtons(Button *exceptThisButton = nullptr);
};

#endif
