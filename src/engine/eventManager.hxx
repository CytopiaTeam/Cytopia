#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include <SDL.h>

#include "engine.hxx"
#include "uiManager.hxx"

class EventManager
{
public:
  EventManager() = default;
  ~EventManager() = default;

  void checkEvents(SDL_Event &event, Engine &engine);

private:
  UIManager &uiManager = UIManager::instance();

  UiElement *lastHoveredElement = nullptr;
  bool dispatchUiEvents(SDL_Event &event);
  bool rightMouseButtonHeldDown = false;
};

#endif