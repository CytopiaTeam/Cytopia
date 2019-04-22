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
  UIManager &m_uiManager = UIManager::instance();

  UiElement *m_lastHoveredElement = nullptr;

  bool rightMouseButtonHeldDown = false;
  bool skipLeftClick = false;
};

#endif