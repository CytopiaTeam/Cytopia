#ifndef OBSERVER_HXX_
#define OBSERVER_HXX_

#include <vector>
#include <memory>
#include <algorithm>
#include "Meta.hxx"

template <typename Data> struct DefaultSubjectDispatch;
template <typename Data, typename DispatchPolicy = DefaultSubjectDispatch<Data>> class Subject;

/**
 * @class     Observer
 * @brief     Allows to listen to another Object's
 *            events
 * @see       https://en.wikipedia.org/wiki/Observer_pattern
 * @example   tests/util/Observer.cxx
 */
template <typename Data> class Observer
{
public:
  using Notification = Data;
  virtual void update(Notification) noexcept = 0;

protected:
  Observer() noexcept = default;
  virtual ~Observer() noexcept = 0;

private:
  friend DefaultSubjectDispatch<Data>;
};

template <typename Data> Observer<Data>::~Observer() noexcept {}
template <typename Data> using ObserverSPtr = std::shared_ptr<Observer<Data>>;
template <typename Data> using ObserverWPtr = std::weak_ptr<Observer<Data>>;

/**
 * @class     DefaultSubjectDispatch
 * @brief     A synchronous dispatch to Observers
 * @tparam    Data The data arguments
 */
template <typename Data> struct DefaultSubjectDispatch
{
  void dispatch(ObserverWPtr<Data> &observer, typename Observer<Data>::Notification &notification)
  {
    if (auto lockedObserver = observer.lock())
    {
      lockedObserver->update(notification);
    }
  }
};

/**
 * @class     Subject
 * @brief     Allows to notify Observers about
 *            events
 * @see       https://en.wikipedia.org/wiki/Observer_pattern
 * @example   tests/util/Observer.cxx
 */
template <typename Data, typename DispatchPolicy> class Subject : DispatchPolicy
{
public:
  using Listener = Observer<Data>;
  using Notification = Data;

  /**
   * @brief   Adds an observer to listen to Subject's events
   * @param   observer the observer to add
   */
  void addObserver(ObserverWPtr<Data> observer) { m_Observers.emplace_back(std::move(observer)); }

  /**
   * @brief Remove all non valid observers.
   */
  virtual void prune()
  {
    m_Observers.erase(
        std::remove_if(m_Observers.begin(), m_Observers.end(), [](ObserverWPtr<Data> wPtr) { return wPtr.expired(); }),
        m_Observers.end());
  }

private:
  std::vector<ObserverWPtr<Data>> m_Observers;

protected:
  Subject(DispatchPolicy dispatch = {}) : DispatchPolicy(dispatch) {}

  using DispatchPolicy::dispatch;

  /**
   * @brief notifies all affected observers
   * @param notification the data to be sent to observers
   */
  void notifyObservers(Notification &&notification) noexcept
  {
    prune();

    for (auto &observer : m_Observers)
    {
      if (mustNotify(observer, notification))
      {
        dispatch(observer, notification);
      }
    }
  }

  virtual ~Subject() noexcept = 0;

  /**
   * @returns true if Subject should notify the observer
   * @param   observer the observer to be notified
   * @param   data the data to be sent to observer
   */
  virtual bool mustNotify(ObserverWPtr<Data> &observer, const Notification &data) const noexcept { return true; }
};

template <typename Data, typename DispatchPolicy> Subject<Data, DispatchPolicy>::~Subject() noexcept {}

#endif
