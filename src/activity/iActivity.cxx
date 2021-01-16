#include "iActivity.hxx"
#include "../engine/MessageQueue.hxx"
#include "../Events.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"
#include "../view/Window.hxx"

iActivity::iActivity(GameService::ServiceTuple &context, Window &w) : GameService(context), m_Window(w) {}

iActivity::~iActivity() = default;

void iActivity::activitySwitch(ActivityType type)
{
  GetService<UILoopMQ>().push(ActivitySwitchEvent{type});
  GetService<GameLoopMQ>().push(ActivitySwitchEvent{type});
}

Window &iActivity::getWindow() noexcept { return m_Window; }

void iActivity::bindMouse()
{
  MouseController &mouseCtrl = GetService<MouseController>();

  mouseCtrl.removeHandlers();

  for (auto it = m_Controllers.cbegin(); it < m_Controllers.cend(); ++it)
  {
    if (auto handler = dynamic_cast<iMouseHandler *>(it->get()))
    {
      mouseCtrl.addHandler(handler);
    }
  }
}
