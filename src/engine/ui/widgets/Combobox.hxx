#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include <memory>

#include "../basics/UIElement.hxx"
#include "Text.hxx"


enum class TextFieldAlignment
{
  LEFT,
  RIGHT,
  CENTERED
};

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

  size_t count() const { return m_items.size(); };

  /// the text displayed in the Combobox
  std::string activeText;

  int hoveredID = -1;

  TextFieldAlignment textAlignment = TextFieldAlignment::CENTERED;
  void registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) override;

  /** @brief Clears the element's items
  * clears the items in the dropdown menu of the comboBox
  */
  void clear() { m_items.clear(); }

  std::string getTextFromID(int id) const;

private:
  /// the active element's ID
  int m_activeID = 0;
  /// represents the dropdownMenu
  SDL_Rect m_dropdownRect;
  /// represents the whole UIElement including the opened menu
  SDL_Rect m_wholeElementRect;
  // a rect is drawn beneath the current text to hover it
  SDL_Rect m_highlightingRect;

  /// the items in the dropdown menu as Text widgets
  std::vector<std::unique_ptr<Text>> m_items;
  /// the current item as a Text widget
  std::unique_ptr<Text> m_selectedItem;
  /// height of each item
  int m_itemHeight = 0;

  bool m_isMenuOpened = false;

  void centerTextLabel() const;

  bool checkBoundaries(int x, int y) const;

  // Signals
  Signal::Signal<void(UIElement *sender)> clickSignalSender;
};

#endif
