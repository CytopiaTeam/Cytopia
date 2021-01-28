#ifndef SELECTION_LAYOUT_MODEL_HXX
#define SELECTION_LAYOUT_MODEL_HXX

#include "DataModel.hxx"

using SelectionLayoutNotifications = TypeList<float, int>;

class SelectionLayoutModel : public DataModel<SelectionLayoutNotifications>
{
private:
  float m_ScrollPosition = 0.0f;
  int m_SelectedIndex = -1;

public:
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
};

using SelectionLayoutListener = SelectionLayoutModel::Listener;

#endif // SELECTION_LAYOUT_MODEL_HXX