#ifndef PRIORITY_QUEUE_HXX_
#define PRIORITY_QUEUE_HXX_

#include <vector>

/**
  * @brief Wrap priority queue class to extend functionality.
  */
template <typename T> class PriorityQueue
{
  std::vector<T> m_container;

public:
  using Iterator = decltype(m_container.begin());

  Iterator begin() { return m_container.begin(); }

  Iterator end() { return m_container.end(); }
  /**
    * @brief Check whether queue is empty.
    * @return true if queue is empty otherwise false.
    */
  bool isEmpty() { return m_container.empty(); }

  /**
    * @brief Get top element from the queue.
    *        Task which expire first.
    * @return Top task from queue.
    */
  T &top() { return m_container.front(); }

  /**
    * @brief Add new task to the queue.
    */
  void add(T &&element);

  /**
    * @brief Remove first task from the queue.
    */
  void pop();

  /**
    * @brief Remove task from queue.
    * @param position Position of element to be removed.
    */
  void remove(Iterator position);

  /**
    * @brief Remove all elements from queue.
    */
  void clear(void);
};

#include "PriorityQueue.inl.hxx"

#endif // PRIORITY_QUEUE_HXX_