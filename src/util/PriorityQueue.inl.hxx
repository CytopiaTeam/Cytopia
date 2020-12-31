template <typename T> void PriorityQueue<T>::add(T &&element)
{
  m_container.emplace_back(element);
  std::push_heap(m_container.begin(), m_container.end(), std::greater<T>{});
}

template <typename T> void PriorityQueue<T>::pop(void)
{
  std::pop_heap(m_container.begin(), m_container.end(), std::greater<T>{});
  m_container.pop_back();
}

template <typename T> void PriorityQueue<T>::remove(Iterator position)
{
  m_container.erase(position);
  std::make_heap(m_container.begin(), m_container.end(), std::greater<T>{});
}