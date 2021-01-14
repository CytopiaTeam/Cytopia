#ifndef UI_MQ_DISPATCHER
#define UI_MQ_DISPATCHER

#include "Observer.hxx"
#include "MessageQueue.hxx"

/**
 * @class   UIMQDispatcher
 * @brief   An asynchronous dispatch to Observers
 *          based on a UIMessageQueue processed on another thread
 * @details This dispatch policy allows for the GameLoop to asynchronously
 *          update the Views while modifying iModels
 * @tparam  DataArgs The data arguments
 */
template <typename DataArgs> struct UIMQDispatcher
{
  class UILoopMQ &loop;
  void dispatch(ObserverWPtr<DataArgs> observer, typename Observer<DataArgs>::Notification notification)
  {
    loop.push(UIChangeEvent{observer, std::move(notification)});
  }
};

#endif // UI_MQ_DISPATCHER
