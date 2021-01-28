#include "SelectionLayoutModel.hxx"

void SelectionLayoutModel::moveScrollPos(float steps)
{
  m_ScrollPosition = std::clamp(m_ScrollPosition + steps, 0.f, 1.f);
  notifyObservers(Notification{m_ScrollPosition});
}

void SelectionLayoutModel::setSelectIndex(int idx)
{
  if (m_SelectedIndex != idx)
  {
    m_SelectedIndex = idx;
    notifyObservers(Notification{m_SelectedIndex});
  }
}

float SelectionLayoutModel::getScrollPos() { return m_ScrollPosition; }