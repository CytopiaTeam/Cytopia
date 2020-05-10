#ifndef UI_EVENTS_HXX
#define UI_EVENTS_HXX

#include <functional>

#include "../util/Observer.hxx"
/**
 * @brief Events to be handled by the UI Loop
 */
using UIEvents = TypeList<
  struct TerminateEvent,
  struct WindowResizeEvent,
  struct WindowRedrawEvent,
  struct UIChangeEvent /* Add UI Events here */>;

/**
 * @brief A change in the window's size
 */
struct WindowResizeEvent
{
};

/**
 * @brief A request to redraw the Window
 */
struct WindowRedrawEvent
{
};

/**
 * @brief A change in the UI
 */
struct UIChangeEvent
{
  std::function<void()> apply;
  template <typename DataArgs>
  UIChangeEvent(ObserverSPtr<DataArgs> observer, typename Observer<DataArgs>::Notification && notification) : 
    apply([observer, notification = std::move(notification)]() { observer->update(std::move(notification)); })
    {}
};

#endif // UI_EVENTS_HXX
