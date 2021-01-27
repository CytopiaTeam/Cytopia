#include "EventHandler.hxx"
#include "UIEvents.hxx"
#include "SDL_events.h"

void EventHandler::loop(UILoopMQ &uiLoopMq, MouseController &mouseCtrl)
{
  SDL_Event event;
  for (;;)
  {
    while (SDL_WaitEvent(&event) != 0)
    {
      switch (event.type)
      {
      case SDL_QUIT:
      {
        return;
      }
      case SDL_WINDOWEVENT:
      {
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
          uiLoopMq.push(WindowResizeEvent{});
          continue;
        }
        default:
          continue;
        }
      }
      case SDL_MOUSEMOTION:
      {
        mouseCtrl.handleEvent(MousePositionEvent{event.motion.x, event.motion.y, 0, 0});
        continue;
      }
      case SDL_MOUSEBUTTONDOWN:
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          mouseCtrl.leftButtonDown(ClickEvent{event.button.x, event.button.y});
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
          mouseCtrl.middleButtonDown(ClickEvent{event.button.x, event.button.y});
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          mouseCtrl.rightButtonDown(ClickEvent{event.button.x, event.button.y});
        }
        continue;
      }
      case SDL_MOUSEBUTTONUP:
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          mouseCtrl.leftButtonUp(ClickEvent{event.button.x, event.button.y});
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
          mouseCtrl.middleButtonUp(ClickEvent{event.button.x, event.button.y});
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          mouseCtrl.rightButtonUp(ClickEvent{event.button.x, event.button.y});
        }
        continue;
      }
      case SDL_MOUSEWHEEL:
      {
        if ((event.wheel.x != 0) || (event.wheel.y != 0))
        {
          mouseCtrl.scroll(ScrollEvent{(event.wheel.direction == SDL_MOUSEWHEEL_NORMAL) ? event.wheel.x : -event.wheel.x,
                                       (event.wheel.direction == SDL_MOUSEWHEEL_NORMAL) ? event.wheel.y : -event.wheel.y});
        }
      }
      default:
        continue;
      }
    }
  }
}