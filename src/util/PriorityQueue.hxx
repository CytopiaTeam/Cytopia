#ifndef PRIORITY_QUEUE_HXX_
#define PRIORITY_QUEUE_HXX_

#include <vector>

/**
  * @brief Priority queue with removing functionality.
  */
template <typename T> class PriorityQueue
{
  std::vector<T> m_container;

public:
  using Iterator = typename std::vector<T>::iterator;

  /**
    * @return Iterator to beginning of queue.
    */
  Iterator begin() { return m_container.begin(); }

  /**
    * @return Iterator to end of queue.
    */
  Iterator end() { return m_container.end(); }

  /**
    * @brief Check whether queue is empty.
    * @return true if queue is empty otherwise false.
    */
  bool isEmpty() { return m_container.empty(); }

  /**
    * @brief Get top element from the queue.
    * @return Top element from queue.
    */
  T &top() { return m_container.front(); }

  /**
    * @brief Add new element to the queue.
    */
  void add(T &&element);

  /**
    * @brief Remove first element from the queue (from top).
    */
  void pop();

  /**
    * @brief Remove task from queue.
    * @param position Iterator of element to be removed.
    */
  void remove(Iterator position);

  /**
    * @brief Remove all elements from queue.
    */
  void clear(void) { m_container.clear(); }
};

#include "PriorityQueue.inl.hxx"

#endif // PRIORITY_QUEUE_HXX_