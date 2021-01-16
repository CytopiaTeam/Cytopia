#ifndef EVENT_HANDLER_HXX
#define EVENT_HANDLER_HXX

#include "../engine/MessageQueue.hxx"
#include "../services/MouseController.hxx"

class EventHandler
{
public:
  /**
   * @brief Handle SDL events. Will not return till game is quit.
   * @param uiLoopMq UI loop queue.
   * @param mouseCtrl Mouse controller.
   */
  static void loop(UILoopMQ &uiLoopMq, MouseController &mouseCtrl);
};

#endif // EVENT_HANDLER_HXX