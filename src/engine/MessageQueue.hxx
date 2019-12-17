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
template <typename Semaphore> using Lock = std::unique_lock<Semaphore>;

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
   * @details Blocks the thread until an event is received
   * @returns an Enumerable of Events on the Queue 
   * @post the Queue is emptied
   * @post Enumerable contains at least one Event
   */
  Enumerable getEnumerable(void);

  /* These operators are deleted to prevent race hazards */
  MessageQueue(const MessageQueue &) = delete;
  MessageQueue &operator=(const MessageQueue &) = delete;

  MessageQueue() = default;
  MessageQueue(MessageQueue &&) = default;
  MessageQueue &operator=(MessageQueue &&) = default;

private:
  /**
   * @pre This thread owns a m_Semaphore Lock
   * @returns true if the queue contains an event
   */
  bool wakeCondition(void) const noexcept;

  MonitorUPtr m_OnEvent = std::make_unique<Monitor>();
  MutexUPtr m_Semaphore = std::make_unique<Mutex>();
  Deque<Event> m_Queue;
};

#include "MessageQueue.inl.hxx"

/**
 * @brief UI Actor's message queue
 */
class UILoopMQ : public MessageQueue<typename VariantType<UIEvents>::type>
{
};

/**
 * @brief Game loop Actor's message queue
 */
class GameLoopMQ : public MessageQueue<typename VariantType<GameEvents>::type>
{
};

#endif
