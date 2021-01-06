template <typename T, typename Comparator> inline bool PriorityQueue<T, Comparator>::empty() const noexcept
{
  return m_container.empty();
}

template <typename T, typename Comparator>
inline typename PriorityQueue<T, Comparator>::reference PriorityQueue<T, Comparator>::top()
{
  return m_container.front();
}

template <typename T, typename Comparator>
inline typename PriorityQueue<T, Comparator>::const_reference PriorityQueue<T, Comparator>::top() const
{
  return m_container.front();
}

template <typename T, typename Comparator> void PriorityQueue<T, Comparator>::push(value_type &&element)
{
  m_container.emplace_back(std::move(element));
  std::push_heap(m_container.begin(), m_container.end(), Comparator{});
}

template <typename T, typename Comparator> void PriorityQueue<T, Comparator>::push(const value_type &element)
{
  m_container.emplace_back(element);
  std::push_heap(m_container.begin(), m_container.end(), Comparator{});
}

template <typename T, typename Comparator> void PriorityQueue<T, Comparator>::pop(void)
{
  std::pop_heap(m_container.begin(), m_container.end(), Comparator{});
  m_container.pop_back();
}

template <typename T, typename Comparator>
template <typename Predicate>
typename PriorityQueue<T, Comparator>::size_type PriorityQueue<T, Comparator>::erase_if(Predicate &&predicate)
{
  auto removedIter = std::remove_if(m_container.begin(), m_container.end(), std::forward<Predicate>(predicate));
  PriorityQueue<T, Comparator>::size_type numOfRemovedElements = 0;

  if (removedIter != m_container.end())
  {
    numOfRemovedElements = m_container.end() - removedIter;
    m_container.erase(removedIter, m_container.end());
    std::make_heap(m_container.begin(), m_container.end(), Comparator{});
  }

  return numOfRemovedElements;
}

template <typename T, typename Comparator> inline void PriorityQueue<T, Comparator>::clear(void) noexcept { m_container.clear(); }
