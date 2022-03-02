#ifndef MESSAGE_QUEUE_HXX_
#define MESSAGE_QUEUE_HXX_

#include <condition_variable>
#include <mutex>
#include <deque>
#include <vector>
#include <functional>
#include <ciso646>

#include "../Events.hxx"

using Monitor = std::condition_variable;
using Mutex = std::mutex;
using MonitorUPtr = std::unique_ptr<Monitor>;
using MutexUPtr = std::unique_ptr<Mutex>;
template <typename Type> using Deque = std::deque<Type>;
template <typename LockType> using Lock = std::unique_lock<LockType>;

/**
 * @brief A Thread safe MessageQueue
 * @example tests/util/MessageQueue.cxx
 */
template <typename Event> class MessageQueue
{

public:
  using Enumerable = Deque<Event>;

  /**
   * @brief Push an event on the Queue
   * @post All waiting threads are notified
   */
  void push(Event &&event);

  /**
   * @returns true if the queue contains a message
   * @details This method is non-blocking
   */
  bool peek(void) noexcept;

  /**
   * @details Blocks the thread until an event is received.
   * @returns an Enumerable of the Queue Events.
   * @post the Queue is emptied
   * @post Enumerable contains at least one Event
   */
  Enumerable getEnumerable(void);

  /**
   * @details Blocks the thread until an event is received or timeout occurred.
   * @returns an Enumerable of the Queue Events if at least one event received before timeout occurred, otherwise empty enumerable of Queue Events.
   * @tparam Duration the type of the Duration
   * @param the duration value
   */
  template <typename Duration> Enumerable getEnumerableTimeout(Duration &&duration);

  /* These operators are deleted to prevent race hazards */
  MessageQueue(const MessageQueue &) = delete;
  MessageQueue &operator=(const MessageQueue &) = delete;

  MessageQueue() = default;
  MessageQueue(MessageQueue &&) = default;
  MessageQueue &operator=(MessageQueue &&) = default;

private:
  /**
   * @pre This thread owns a m_mutex Lock
   * @returns true if the queue contains an event
   */
  bool wakeCondition(void) const noexcept;

  MonitorUPtr m_onEvent = std::make_unique<Monitor>();
  MutexUPtr m_mutex = std::make_unique<Mutex>();
  Deque<Event> m_queue;

  Enumerable swapQueue(void);
};

#include "MessageQueue.inl.hxx"

/**
 * @brief UI Actor's message queue
 */
class UILoopMQ : public MessageQueue<typename UIEvents::VariantType>
{
};

/**
 * @brief Game loop Actor's message queue
 */
class GameLoopMQ : public MessageQueue<typename GameEvents::VariantType>
{
};

#endif
