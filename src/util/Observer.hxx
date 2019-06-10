#ifndef OBSERVER_HXX_
#define OBSERVER_HXX_

#include <memory>
#include <vector>

template <typename T>
using List = std::vector<T>;
template <typename ...DataArgs>
class Subject;

/**
 * @class Observer
 * @brief Allows to listen to another Object's
 *        events
 * @see https://en.wikipedia.org/wiki/Observer_pattern
 */
template <typename ...DataArgs>
class Observer
{
protected:
  Observer() noexcept = default;
  virtual ~Observer() noexcept = 0;
private:
  void update(DataArgs...) noexcept = 0;
  template <typename ...DataArgs>
  using ObserverPtr = std::shared_ptr<Observer<DataArgs...> >;
};

template <typename ...DataArgs>
inline Observer<DataArgs...>::~Observer() noexcept { }

/**
 * @class Subject
 * @brief Allows to notify Observers about
 *        events
 * @see https://en.wikipedia.org/wiki/Observer_pattern
 */
template <typename ...DataArgs>
class Subject
{
  friend Observer<DataArgs...>;
private:
  List<ObserverPtr> m_Observers;
protected:
  Subject() = default;
  inline void notifyObservers(DataArgs... args) noexcept
  {
    for(ObserverPtr observer : m_Observers)
    {
      observer->update(args...);
    }
  }
  virtual ~Subject() noexcept = 0;
};

template <typename ...DataArgs>
inline Subject<DataArgs...>::~Subject() noexcept { }

#endif
