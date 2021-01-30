#ifndef SELECTION_LAYOUT_MODEL_HXX
#define SELECTION_LAYOUT_MODEL_HXX

#include "DataModel.hxx"

struct SelectItemNotif
{
  int index;
};

struct HoverItemNotif
{
  int index;
};

struct ScrollPosNotif
{
  float position;
};

using SelectionLayoutNotifications = TypeList<SelectItemNotif, HoverItemNotif, ScrollPosNotif>;

class SelectionLayoutModel : public DataModel<SelectionLayoutNotifications>
{
private:
  float m_ScrollPosition = 0.0f;
  int m_SelectedIndex = NOT_SELECTED_INDEX;
  int m_HoverIndex = NOT_SELECTED_INDEX;

public:
  static constexpr int NOT_SELECTED_INDEX = -1;

  using DataModel::DataModel;

  /**
  * @brief Move scroll position.
  * @param steps Scroll position increment.
  */
  void moveScrollPos(float steps);

  /**
  * @brief Set scroll position.
  * @param pos New scroll position.
  */
  void setScrollPos(float pos);

  /**
  * @brief Get scroll position.
  */
  float getScrollPos();

  /**
  * @brief Set selected item index.
  * @return Whether selected item has been changed.
  */
  bool setSelectIndex(int idx);

  /**
  * @brief Set hovering item index.
  */
  void setHoverIndex(int idx);
};

using SelectionLayoutListener = SelectionLayoutModel::Listener;

#endif // SELECTION_LAYOUT_MODEL_HXX