#include "ButtonModel.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

ButtonModel::~ButtonModel() = default;

void ButtonModel::setStatus(ButtonStatus status)
{
  std::swap(m_Status, status);
  notifyObservers(Notification{m_Status});
}
