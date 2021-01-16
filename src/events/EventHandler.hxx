#ifndef EVENT_HANDLER_HXX
#define EVENT_HANDLER_HXX

#include "../engine/MessageQueue.hxx"
#include "../services/MouseController.hxx"

class EventHandler
{
public:
  static void loop(UILoopMQ &uiLoopMq, MouseController &mouseCtrl);
};

#endif // EVENT_HANDLER_HXX