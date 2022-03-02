#include "LOG.hxx"

template <typename DelayType, typename PeriodType>
inline GameClock::ClockTaskHndl GameClock::addRealTimeClockTask(ClockCbk cbk, DelayType delay, PeriodType period)
{
  assert((cbk != nullptr) && (TimePoint(delay) >= TimePointZero) && (TimePoint(period) >= TimePointZero));

  std::lock_guard<std::mutex> lock(m_lock);
  TimePoint delayConverted = (TimePoint)delay;

  // If no delay required call callback immediately
  if (delayConverted == TimePointZero)
  {
    const bool isCanceled = cbk();

    if (isCanceled || ((TimePoint)period == TimePointZero))
    {
      return ClockTaskHndlInvalid;
    }

    delayConverted = (TimePoint)period;
  }

  m_realTimeTasks.push(RealTimeClockTask(cbk, Clock::now() + (delayConverted - TimePointZero), (TimePoint)period - TimePointZero,
                                        ++m_unique_handle));
  return m_unique_handle;
}
