#ifndef DROPDOWNMENU_HXX_
#define DROPDOWNMENU_HXX_

#include "../basics/UIElement.hxx"
#include "Text.hxx"

enum class TextFieldAlignment
{
  LEFT,
  RIGHT,
  CENTERED
};

/**
 * @brief A DropdownMenu GUI Widget
 */
class DropdownMenu : public UIElement
{
public:
  DropdownMenu() = default;
  DropdownMenu(const SDL_Rect &uiElementRect);

  ~DropdownMenu() override = default;

  void addItem(const std::string &text);

  std::string getTextFromID(int id) const;

  SDL_Rect getSize() const { return m_uiElementRect; };

  void setPosition(int x, int y) override;

  void draw() override;

  bool onMouseButtonUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  /** @brief Get the number of items in the menu
  * @returns number of items in the menu
  */
  size_t count() const { return m_numItems; };
  bool centerText = true;

  int selectedID = -1;
  int hoveredID = -1;

  TextFieldAlignment textAlignment = TextFieldAlignment::CENTERED;

  /// deletes all items from the selection menu
  void clear() { m_items.clear(); }

private:
  size_t m_numItems = 0; ///< the number of items in the dropdown menu

  /// the items in the dropdown menu as Text widgets
  std::vector<std::unique_ptr<Text>> m_items;

  /// height of each item
  int m_itemHeight = 0;

  // a rect is drawn beneath the current text to hover it
  SDL_Rect m_highlightingRect;
};

#endif