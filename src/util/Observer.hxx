#ifndef OBSERVER_HXX_
#define OBSERVER_HXX_

#include <memory>
#include <forward_list>

template <typename T>
using LinkedList = std::forward_list<T>;
template <typename ...DataArgs>
class Subject;

/**
 * @class Observer
 * @brief Allows to listen to another Object's
 *        events
 * @see https://en.wikipedia.org/wiki/Observer_pattern
 * @example test/util/Observer.cxx
 */
template <typename ...DataArgs>
class Observer
{
protected:
  Observer() noexcept = default;
  virtual ~Observer() noexcept = 0;
private:
  virtual void update(DataArgs...) noexcept = 0;
  friend Subject<DataArgs...>;
};

template <typename ...DataArgs>
inline Observer<DataArgs...>::~Observer() noexcept { }
template <typename ...DataArgs>
using ObserverWPtr = std::weak_ptr<Observer<DataArgs...> >;
template <typename ...DataArgs>
using ObserverSPtr = std::shared_ptr<Observer<DataArgs...> >;

/**
 * @class Subject
 * @brief Allows to notify Observers about
 *        events
 * @see https://en.wikipedia.org/wiki/Observer_pattern
 * @example test/util/Observer.cxx
 */
template <typename ...DataArgs>
class Subject
{
private:
  LinkedList<ObserverWPtr<DataArgs...> > m_Observers;
protected:
  Subject() = default;
  using ObsIterator = typename LinkedList<ObserverWPtr<DataArgs...> >::iterator;
  inline void notifyObservers(DataArgs... args) noexcept
  {
    ObsIterator it = m_Observers.before_begin();
    ObsIterator old;
    while(it != m_Observers.end())
    {
      old = it;
      if(++it == m_Observers.end()) continue;
      if(mustNotify(it, args...) and auto observer = it->lock())
      {
        observer->update(args...);
      }
      else if(it->expired())
      {
        m_Observers.erase_after(old);
        it = old;
        continue;
      }
    }
  }
  virtual ~Subject() noexcept = 0;
  virtual inline bool mustNotify(ObsIterator, const DataArgs& ...) const noexcept { return true; }
public:
  inline virtual void addObserver(ObserverSPtr<DataArgs...> observer)
  {
    m_Observers.emplace_front(observer);
  }
};

template <typename ...DataArgs>
inline Subject<DataArgs...>::~Subject() noexcept { }

#endif
