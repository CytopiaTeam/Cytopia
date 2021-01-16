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
        continue;
      }
      case SDL_MOUSEMOTION:
      {
        mouseCtrl.handleEvent(MousePositionEvent{event.motion.x, event.motion.y, 0, 0});
        continue;
      }
      case SDL_MOUSEBUTTONDOWN:
      {
        uint8_t buttonId = 0;

        LOG(LOG_ERROR) << (uint32_t)event.button.button;

        if (event.button.button == SDL_BUTTON_LEFT)
        {
          buttonId = ClickEvent::MouseButtonID::Left;
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
          buttonId = ClickEvent::MouseButtonID::Middle;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          buttonId = ClickEvent::MouseButtonID::Right;
        }

        if (buttonId)
        {
          mouseCtrl.handleEvent(
              ClickEvent{event.button.x, event.button.y, (uint8_t)(buttonId | ClickEvent::MouseButtonState::Pressed)});
        }
        continue;
      }
      case SDL_MOUSEBUTTONUP:
      {
        uint8_t buttonId = 0;

        if (event.button.button == SDL_BUTTON_LEFT)
        {
          buttonId = ClickEvent::MouseButtonID::Left;
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
          buttonId = ClickEvent::MouseButtonID::Middle;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          buttonId = ClickEvent::MouseButtonID::Right;
        }

        if (buttonId)
        {
          mouseCtrl.handleEvent(
              ClickEvent{event.button.x, event.button.y, (uint8_t)(buttonId | ClickEvent::MouseButtonState::Released)});
        }

        continue;
      }
      default:
        continue;
      }
    }
  }
}