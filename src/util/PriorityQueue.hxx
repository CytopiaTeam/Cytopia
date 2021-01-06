#ifndef PRIORITY_QUEUE_HXX_
#define PRIORITY_QUEUE_HXX_

#include <vector>
#include <algorithm>

/**
  * @brief Priority queue with erase element functionality.
  */
template <typename T, typename Comparator = std::less<T>> class PriorityQueue
{
public:
  using Container = std::vector<T>;
  using container_type = Container;
  using value_compare = Comparator;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using allocator_type = typename Container::allocator_type;
  using different_type = typename Container::difference_type;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;

  /**
    * @brief Check whether queue is empty.
    * @return true if queue is empty otherwise false.
    */
  bool empty() const noexcept;

  /**
    * @brief Get top element from the queue.
    * @return Top element from queue.
    */
  reference top();

  /**
    * @brief Get top element from the queue.
    * @return Top element from queue.
    */
  const_reference top() const;

  /**
    * @brief Add new element to the queue.
    */
  void push(value_type &&element);

  /**
    * @brief Add new element to the queue.
    */
  void push(const value_type &element);

  /**
    * @brief Remove first element from the queue (from top).
    */
  void pop();

  /**
    * @brief Remove all elements from queue for which predicate returns true.
    * @param predicate Unary predicate which returns true if element should be removed.
    */
  template <typename Predicate> size_type erase_if(Predicate &&predicate);

  /**
    * @brief Remove all elements from queue.
    */
  void clear(void) noexcept;

private:
  Container m_container;
};

#include "PriorityQueue.inl.hxx"

#endif // PRIORITY_QUEUE_HXX_