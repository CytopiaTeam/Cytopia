#ifndef TRANSITIVE_MODEL_HXX_
#define TRANSITIVE_MODEL_HXX_

#include "Meta.hxx"
#include "Observer.hxx"

#include <unordered_set>
#include <map>
#include <ciso646>

template <typename Key> using Set = std::unordered_set<Key>;
template <typename Key, typename Value, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<std::pair<const Key, Value>>>
using TreeMap = std::map<Key, Value, Compare, Allocator>;

/**
 * @brief Represents the change between one state to another
 * @example tests/util/TransitiveModel.cxx
 */
template <typename Model> struct Transition
{
  static_assert(std::is_enum<typename Model::Operations>::value, "Model::Operations must be an enum-type");
  static_assert(std::is_class<typename Model::OperationTypes>::value, "Model::OperationTypes must be a type list");
  using TransitionData = typename VariantType<typename Model::OperationTypes>::type;
  using TransitionType = typename Model::Operations;
  TransitionData data;
  inline constexpr TransitionType getType() const { return static_cast<TransitionType>(data.index()); }
  template <typename OperationData, typename = std::enable_if<ContainsType<typename Model::OperationTypes, OperationData>, void>>
  explicit Transition(OperationData data) : data(data)
  {
  }
  inline constexpr bool operator==(const Transition &other) const noexcept { return data == other.data; }
};

/* Deduction guide for constructors */
template <typename Data> explicit Transition(Data data)->Transition<typename Data::MyModelType>;

/**
 * @details A subject model that is tightly coupled with its operations.
 *          Specifically, for each possible transition in that Model 
 *          there exists a Transition<> type to represent it.
 *          Observers to this model must update(...) with those
 *          Transition objects, but they can only be subscribed to specific
 *          events
 * @example tests/util/TransitiveModel.cxx
 */
template <typename Model> class TransitiveModel : protected Model, public Subject<Transition<Model>>
{
protected:
  TransitiveModel() = default;

public:
  using Event = Transition<Model>;
  using TransitionType = typename Event::TransitionType;

  static_assert(std::is_enum_v<TransitionType>, "TransitionType must be an enum type");

  template <typename... Args> inline void subscribe(ObserverSPtr<Event> observer, Args... filters)
  {
    addObserver(observer);
    (m_Filters[observer].insert(filters), ...);
  }

private:
  using Subject<Event>::addObserver;
  using FilterType = TreeMap<ObserverWPtr<Event>, Set<typename Event::TransitionType>, std::owner_less<ObserverWPtr<Event>>>;
  FilterType m_Filters;

  virtual inline bool mustNotify(ObserverWPtr<Event> observer, const Event &event) noexcept final
  {
    if (observer.expired())
    {
      return false;
    }
    return m_Filters[observer].count(event.getType()) == 1;
  }

  virtual inline void onObserverExpired(void) noexcept override
  {
    /* We are responsible for removing the ObserverWPtrs */
    using Iterator = typename FilterType::iterator;
    for (Iterator it = m_Filters.begin(); it != m_Filters.end(); ++it)
    {
      if (std::get<0>(*it).expired())
      {
        m_Filters.erase(it);
      }
    }
  }
};

#endif
