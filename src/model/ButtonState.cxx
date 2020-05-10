#include "ButtonState.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"

ButtonState::~ButtonState() = default;

void ButtonState::setStatus(ButtonStatus status)
{
  std::swap(m_Status, status);
  notifyObservers(Notification{m_Status});
}
