#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include <memory>

#include "../basics/UIElement.hxx"
#include "DropdownMenu.hxx"

class ComboBox : public UIElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() override = default;

  bool onMouseButtonUp(const SDL_Event &event) override;
  bool onMouseButtonDown(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;

  void draw() override;

  void setPosition(int x, int y) override;

  bool isMouseOver(int x, int y) override;
  bool isMouseOverHoverableArea(int x, int y) override;

  void addElement(const std::string &text);

  /** \brief Get ID of selected element
  * returns the ID of the selected element in the comboBox
  * @return selected ID
  */
  int getActiveID() const { return m_activeID; };

  /** @brief Set ID of selected element
  * sets the ID of the selected element in the comboBox
  * @param ID the value to set the ID to
  */
  void setActiveID(int ID);

  size_t count() const { return m_dropdownMenu->count(); };
  std::string activeText;

  void registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) override;

  /** @brief Clears the element's items
  * clears the items in the dropdown menu of the comboBox
  */
  void clear() { m_dropdownMenu->clear(); }

private:
  /// the active element's ID
  int m_activeID = 0;
  /// represents the dropdownMenu
  SDL_Rect m_dropdownRect;
  /// represents the whole UIElement including the opened menu
  SDL_Rect m_wholeElementRect;
  /// pointer to the DropdownMenu of this comboBox
  std::unique_ptr<DropdownMenu> m_dropdownMenu;

  bool m_isMenuOpened = false;

  std::unique_ptr<Text> m_selectedItem;

  void centerTextLabel() const;

  bool checkBoundaries(int x, int y) const;

  // Signals
  Signal::Signal<void(UIElement *sender)> clickSignalSender;
};

#endif
