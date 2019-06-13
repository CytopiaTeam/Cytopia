#ifndef TRANSITIVE_MODEL_HXX_
#define TRANSITIVE_MODEL_HXX

#include <type_traits>

#include "Observer.hxx"

/**
 * @struct Transition
 * @brief Represents the change between one state to another
 * @example tests/util/TransitiveModel.cxx
 */
template <typename Model, typename Operations = typename Model::Operations, 
         template<typename... Types> typename OperationTypes = template<typename... Args> typename Model::OperationTypes>
struct Transition<Model>
{
  static_assert(std::is_enum<Operations>::value, "Model::Operations must be an enum-type");
  static_assert(std::is_class<OperationTypes>::value, "Model::OperationTypes must be a type list");
  using TransitionData = std::variant<Types...>;
  using TransitionType = typename Model::Operations;
  TransitionType type;
  TransitionData data;
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
class TransitiveModel : protected Model, public Subject< Transition<Model> > 
{
  void subscribe(typename Model::Operations... filters);
private:
  using Subject<Transition<Model> >::addObserver;
  using Iterator = typename LinkedList<ObserverWPtr<DataArgs...> >::iterator;
  inline final bool mustNotify(Iterator it, Transition<Model> event) noexcept const;
};

#endif
