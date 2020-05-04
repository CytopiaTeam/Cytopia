#ifndef DATA_MODEL_HXX
#define DATA_MODEL_HXX

#include "iModel.hxx"
#include "../util/Observer.hxx"
#include "../engine/MessageQueue.hxx"
#include "../engine/UIMQDispatcher.hxx"

/**
 * @class   DataModel
 * @tparam  DataList    A TypeList of UpdateEvents that form a Notification
 */
template <typename DataList>
struct DataModel : public iModel, public Subject<typename VariantType<DataList>::type, UIMQDispatcher<DataArgs>>
{
  explicit DataModel(UILoopMQ &mq) : Subject<DataArgs, UIMQDispatcher<DataArgs>>(UIMQDispatcher<DataArgs>{mq}) {}
};

#endif // DATA_MODEL_HXX
