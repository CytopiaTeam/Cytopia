#include "iActivity.hxx"
#include "../engine/MessageQueue.hxx"
#include "../events/UIEvents.hxx"

iActivity::iActivity(GameService::ServiceTuple & context, Window & w) : 
  GameService(context), 
  m_Window(w) 
{
}

iActivity::~iActivity() = default;

void iActivity::activitySwitch(ActivityType type)
{
  GetService<UILoopMQ>().push(ActivitySwitchEvent{type});
  GetService<GameLoopMQ>().push(ActivitySwitchEvent{type});
}

Window & iActivity::getWindow() noexcept
{
  return m_Window;
}

