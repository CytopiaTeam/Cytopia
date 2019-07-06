#ifndef MESSAGE_QUEUE_HXX_
#define MESSAGE_QUEUE_HXX_

#include <condition_variable>
#include <mutex>
#include <deque>
#include <vector>
#include <functional>
#include <ciso646>

using Monitor = std::condition_variable;
using Mutex = std::mutex;
using MonitorUPtr = std::unique_ptr<Monitor>;
using MutexUPtr = std::unique_ptr<Mutex>;
template <typename Type> using Deque = std::deque<Type>;
template <typename Semaphore> using Lock = std::unique_lock<Semaphore>;

/**
 * @threadsafe
 * @brief A Thread safe MessageQueue
 * @example tests/util/MessageQueue.cxx
 */
template <typename Event> class MessageQueue final
{

public:
  using Enumerable = Deque<Event>;

  /**
   * @mutex
   * @brief Push an event on the Queue
   * @post All waiting threads are notified
   */
  void push(Event &&event)
  {
    /* Push the event */
    {
      Lock<Mutex> lock(*m_Semaphore);
      m_Queue.push_back(event);
    }
    /* Notify */
    m_OnEvent->notify_all();
  }

  /**
   * @returns true if the queue contains a message
   * @details This method is non-blocking assuming the
   *          thread is not starving
   */
  bool peek(void) noexcept
  {
    Lock<Mutex> lock(*m_Semaphore);
    return m_OnEvent->wait_for(lock, std::chrono::seconds::zero(), std::bind(&MessageQueue::wakeCondition, this));
  }

  /**
   * @mutex
   * @details Blocks the thread until an event is received
   * @returns an Enumerable of Events on the Queue 
   * @post the Queue is emptied
   * @post Enumerable contains at least one Event
   */
  Enumerable getEnumerable(void)
  {
    Lock<Mutex> lock(*m_Semaphore);
    m_OnEvent->wait(lock, std::bind(&MessageQueue::wakeCondition, this));
    Enumerable enumerable;
    std::swap(enumerable, m_Queue);
    return enumerable;
  }

  /* These operators are deleted to prevent race hazards */
  MessageQueue(const MessageQueue &) = delete;
  MessageQueue &operator=(const MessageQueue &) = delete;

  MessageQueue() = default;
  MessageQueue(MessageQueue &&) = default;
  MessageQueue &operator=(MessageQueue &&) = default;

private:
  /**
   * @racehazard
   * @pre This thread owns a m_Semaphore Lock
   * @returns true if the queue contains an event
   */
  inline bool wakeCondition(void) const noexcept { return not m_Queue.empty(); }

  MonitorUPtr m_OnEvent = std::make_unique<Monitor>();
  MutexUPtr m_Semaphore = std::make_unique<Mutex>();
  Deque<Event> m_Queue;
};

#endif
