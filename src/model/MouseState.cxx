#include "MouseState.hxx"
#include "../util/LOG.hxx"

MouseState::~MouseState()
{
  LOG(LOG_DEBUG) << "Destroying MouseState";
}

void MouseState::setPosition(int x, int y, int vx, int vy)
{
  std::swap(m_Model.xPosition, x);
  std::swap(m_Model.yPosition, y);
  std::swap(m_Model.xVelocity, vx);
  std::swap(m_Model.yVelocity, vy);
  notifyObservers(Notification{m_Model});
}

void MouseState::setCursor(CursorType type)
{
  std::swap(m_Model.cursorType, type);
  notifyObservers(Notification{m_Model});
}
