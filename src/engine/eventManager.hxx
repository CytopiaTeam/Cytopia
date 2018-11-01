#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include "SDL2/SDL.h"

#include "engine.hxx"
#include "uiManager.hxx"

class EventManager
{
public:
  EventManager() = default;
  ~EventManager() = default;

  void checkEvents(SDL_Event &event, Engine &engine);

private:
  UIManager &uiManager = UIManager::Instance();

  std::shared_ptr<UiElement> lastHoveredElement = nullptr;
  bool dispatchUiEvents(SDL_Event &event);
};

#endif