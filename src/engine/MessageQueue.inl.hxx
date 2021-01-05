
template <typename Event> void MessageQueue<Event>::push(Event &&event)
{
  /* Push the event */
  {
    Lock<Mutex> lock(*m_mutex);
    m_queue.push_back(event);
  }
  /* Notify */
  m_onEvent->notify_all();
}

template <typename Event> bool MessageQueue<Event>::peek(void) noexcept
{
  Lock<Mutex> lock(*m_mutex);
  return wakeCondition();
}

template <typename Event> typename MessageQueue<Event>::Enumerable MessageQueue<Event>::swapQueue(void)
{
  Enumerable enumerable;
  std::swap(enumerable, m_queue);
  return enumerable;
}

template <typename Event> typename MessageQueue<Event>::Enumerable MessageQueue<Event>::getEnumerable(void)
{
  Lock<Mutex> lock(*m_mutex);
  m_onEvent->wait(lock, std::bind(&MessageQueue::wakeCondition, this));
  return swapQueue();
}

template <typename Event>
template <typename Duration>
typename MessageQueue<Event>::Enumerable MessageQueue<Event>::getEnumerableTimeout(Duration &&duration)
{
  Lock<Mutex> lock(*m_mutex);
  m_onEvent->wait_for(lock, std::forward<Duration>(duration), std::bind(&MessageQueue::wakeCondition, this));
  return swapQueue();
}

template <typename Event> bool MessageQueue<Event>::wakeCondition(void) const noexcept { return not m_queue.empty(); }
