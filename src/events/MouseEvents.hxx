#ifndef MOUSE_EVENTS_HXX
#define MOUSE_EVENTS_HXX

#include "../util/TypeList.hxx"

/**
 * @brief A change in the mouse (or device)'s position
 */
struct MousePositionEvent
{
  int xPosition;
  int yPosition;
  int xVelocity;
  int yVelocity;
};

/**
 * @brief A click interaction
 */
struct ClickEvent
{
  int32_t xPosition;
  int32_t yPosition;
};

/**
 * @brief A scrolling interaction
 */
struct ScrollEvent
{
  int32_t xDisplacement;
  int32_t yDisplacement;
};

using MouseEvents = TypeList<MousePositionEvent, ClickEvent, ScrollEvent>;

#endif // MOUSE_EVENTS_HXX
