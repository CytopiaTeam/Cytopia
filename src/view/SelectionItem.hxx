#ifndef SELECTION_ITEM_HXX
#define SELECTION_ITEM_HXX

#include "iViewElement.hxx"

/**
* @brief It is base class for items used by Selection Layout.
*/
class SelectionItem : public iViewElement
{
protected:
  bool m_IsSelected;
  bool m_IsHovered;
  /**
  * @brief Y offset where item should be rendered. Will be assigned by its layout.
  */
  int m_YOffset;

public:
  void select(bool select) { m_IsSelected = select; }
  void hover(bool hover) { m_IsHovered = hover; }
  void translateY(int y) { m_YOffset = y; }
  void setup(GameService &) noexcept override {}

  SelectionItem() : m_IsSelected{false}, m_IsHovered{false}, m_YOffset{0} {}
  ~SelectionItem() = 0;
};

#endif // SELECTION_ITEM_HXX