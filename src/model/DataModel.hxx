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
struct DataModel : public iModel, public Subject<VariantType<DataList>, UIMQDispatcher<VariantType<DataList>>>
{
  using Dispatcher = UIMQDispatcher<VariantType<DataList>>;
  using SubjectBase = Subject<VariantType<DataList>, Dispatcher>;
  explicit DataModel(UILoopMQ &mq) : SubjectBase(Dispatcher{mq}) {}
};

#endif // DATA_MODEL_HXX
