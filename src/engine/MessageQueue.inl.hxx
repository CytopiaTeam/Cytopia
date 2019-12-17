
template <typename Event> void MessageQueue<Event>::push(Event &&event)
{
  /* Push the event */
  {
    Lock<Mutex> lock(*m_Semaphore);
    m_Queue.push_back(event);
  }
  /* Notify */
  m_OnEvent->notify_all();
}

template <typename Event> bool MessageQueue<Event>::peek(void) noexcept
{
  Lock<Mutex> lock(*m_Semaphore);
  return m_OnEvent->wait_for(lock, std::chrono::seconds::zero(), std::bind(&MessageQueue::wakeCondition, this));
}

template <typename Event> typename MessageQueue<Event>::Enumerable MessageQueue<Event>::getEnumerable(void)
{
  Lock<Mutex> lock(*m_Semaphore);
  m_OnEvent->wait(lock, std::bind(&MessageQueue::wakeCondition, this));
  Enumerable enumerable;
  std::swap(enumerable, m_Queue);
  return enumerable;
}

template <typename Event> bool MessageQueue<Event>::wakeCondition(void) const noexcept { return not m_Queue.empty(); }
