#ifndef TRANSITIVE_MODEL_HXX_
#define TRANSITIVE_MODEL_HXX_

#include "Meta.hxx"
#include "TypeList.hxx"
#include "Observer.hxx"
#include <unordered_set>
#include <map>
#include <ciso646>

template <typename Key> using Set = std::unordered_set<Key>;

template <typename Key, typename Value, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<std::pair<const Key, Value>>>
using TreeMap = std::map<Key, Value, Compare, Allocator>;

/**
 * @class     Transition
 * @brief     Represents the change between one state to another
 * @example   tests/util/TransitiveModel.cxx
 */
template <typename Model> struct Transition
{
  static_assert(std::is_enum<typename Model::Operations>::value, "Model::Operations must be an enum-type");
  static_assert(is_typelist<typename Model::OperationTypes>::value, "Model::OperationTypes must be a type list");
  /* Data associated with the transition */
  using TransitionData = typename Model::OperationTypes::VariantType;
  /* Type of the transition */
  using TransitionType = typename Model::Operations;

  TransitionData data;

  /**
   * @returns The type of the transition
   */
  constexpr TransitionType getType() const { return static_cast<TransitionType>(data.index()); }

  /**
   * @brief   Create a Transition from TransitionData
   */
  template <typename OperationData> explicit Transition(OperationData data) : data(data)
  {
    static_assert(Model::OperationTypes::template ContainsType<OperationData>, "Cannot create a Transition from this data");
  }

  constexpr bool operator==(const Transition &other) const noexcept { return data == other.data; }
};

/* Deduction guide for constructors */
template <typename Data> explicit Transition(Data data) -> Transition<typename Data::MyModelType>;

/**
 * @class     TransitiveModel
 * @details   A Subject equivalent to a finite state machine.
 *            Specifically, for each possible transition in that Model
 *            there exists a Transition<> type to represent it.
 *            Observers to this model must update(...) with those
 *            Transition objects, but they can only be subscribed to specific
 *            transition types
 * @example   tests/util/TransitiveModel.cxx
 */
template <typename Model> class TransitiveModel : protected Model, public Subject<Transition<Model>>
{
protected:
  TransitiveModel() = default;

public:
  using Notification = typename Subject<Transition<Model>>::Notification;
  using TransitionType = typename Notification::TransitionType;

  static_assert(std::is_enum_v<TransitionType>, "TransitionType must be an enum type");

  /**
   * @brief   Subscribe an Observer to get notified of Transitions
   * @param   observer The Observer
   * @param   ilist A list of TransitionTypes to subscribe to. If empty, will subscribe to all Transitions
   */
  void subscribe(ObserverSPtr<Notification> observer, std::initializer_list<typename Notification::TransitionType> ilist = {})
  {
    addObserver(observer);
    if (ilist.size() == 0)
      return;
    m_Filters[observer].insert(ilist.begin(), ilist.end());
  }

  void prune() final
  {
    Subject<Transition<Model>>::prune();

    for (auto it = m_Filters.begin(); it != m_Filters.end(); ++it)
    {
      if (std::get<0>(*it).expired())
      {
        m_Filters.erase(it);
      }
    }
  }

private:
  using Subject<Notification>::addObserver;
  using FilterType = TreeMap<ObserverWPtr<Notification>, Set<typename Notification::TransitionType>,
                             std::owner_less<ObserverWPtr<Notification>>>;
  FilterType m_Filters;

  bool mustNotify(ObserverWPtr<Notification> &observer, const Notification &event) const noexcept final
  {
    if (m_Filters.count(observer) > 0)
    {
      return m_Filters.at(observer).count(event.getType()) == 1;
    }

    return true;
  }
};
#endif
