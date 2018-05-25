#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include "eventManager.hxx"

EventManager::EventManager() 
{
  // Implement event handling here
}

bool EventManager::checkEvents(SDL_Event &event)
{
  bool handled = false;
  // check for UI events first

  switch (event.type)
  {
    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        // check for UI collision here first
        handled = true;
      }
      break;
    default:
      break;
  }

  // return if the event was handled here
  return handled;
}

#endif