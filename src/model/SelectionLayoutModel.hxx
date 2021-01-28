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
  * @param steps Scroll steps.
  */
  void moveScrollPos(float steps);

  /**
  * @brief Get scroll position.
  */
  float getScrollPos();

  /**
  * @brief Set selected item index.
  */
  void setSelectIndex(int idx);

  void setHoverIndex(int idx);
};

using SelectionLayoutListener = SelectionLayoutModel::Listener;

#endif // SELECTION_LAYOUT_MODEL_HXX