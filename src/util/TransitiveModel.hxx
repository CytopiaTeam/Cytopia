#ifndef TRANSITIVE_MODEL_HXX_
#define TRANSITIVE_MODEL_HXX

#include <type_traits>

#include "Observer.hxx"

/**
 * @struct Transition
 * @brief Represents the change between one state to another
 * @example tests/util/TransitiveModel.cxx
 */
template <typename Model>
struct Transition
{
  static_assert(std::is_enum<Model::Operations>::value, "Model::Operations must be an enum-type");
  static_assert(std::is_class<Model::OperationTypes>::value, "Model::OperationTypes must be a templated struct");
  
  template<typename Args...>
  struct ArgType
  {
    static_assert(std::is_Model::OperationTypes, "Model::OperationTypes must be a templated struct");
    typename Model::OperationTypes<Args...>;
    using TransitionData = std::variant<Args...>;
  };

};

/**
 * @class TransitiveModel
 * @brief A subject model that is tightly coupled with its operations.
 *        Specifically, for each possible transition in that Model 
 *        there exists a Transition<> type to represent it.
 *        Observers to this model must update(...) with those
 *        Transition objects, but they can only be subscribed to specific
 *        events
 * @example tests/util/TransitiveModel.cxx
 */
template <typename Model>
class TransitiveModel : protected Data, public Subject< Transition<Model> > 
{
  void subscribe(Model::Operations... filters);
private:
  using Subject<Transition<Data> >::addObserver;
  using Iterator = typename LinkedList<ObserverWPtr<DataArgs...> >::iterator;
  inline final bool mustNotify(Iterator it, Transition<Model> event) noexcept const;
};

#endif
