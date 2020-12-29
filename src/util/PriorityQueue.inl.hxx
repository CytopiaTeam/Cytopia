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

//template <typename T> bool PriorityQueue<T>::remove(T element)
//{
//  auto task = std::find_if(m_container.begin(), m_container.end(), [element](auto it) { return it == element; });
//
//  if (task != m_container.end())
//  {
//    this->m_container.erase(task);
//    std::make_heap(m_container.begin(), m_container.end(), std::greater<T>{});
//    return true;
//  }
//
//  return false;
//}

template <typename T> void PriorityQueue<T>::clear(void) { m_container.clear(); }