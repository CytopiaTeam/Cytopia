#include "SelectionLayoutModel.hxx"

void SelectionLayoutModel::moveScrollPos(float steps)
{
  setScrollPos(m_ScrollPosition + steps);
}

void SelectionLayoutModel::setScrollPos(float pos)
{
  m_ScrollPosition = std::clamp(pos, 0.f, 1.f);
  notifyObservers(Notification{ScrollPosNotif{m_ScrollPosition}});
}

bool SelectionLayoutModel::setSelectIndex(int idx)
{
  if (m_SelectedIndex != idx)
  {
    m_SelectedIndex = idx;
    notifyObservers(Notification{SelectItemNotif{m_SelectedIndex}});
    return true;
  }

  return false;
}

void SelectionLayoutModel::setHoverIndex(int idx)
{
  if (m_HoverIndex != idx)
  {
    m_HoverIndex = idx;
    notifyObservers(Notification{HoverItemNotif{m_HoverIndex}});
  }
}

float SelectionLayoutModel::getScrollPos() { return m_ScrollPosition; }