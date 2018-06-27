#ifndef EVENTMANAGER_HXX_
#define EVENTMANAGER_HXX_

#include "SDL2/SDL.h"

#include "uiManager.hxx"
#include "engine.hxx"

class EventManager
{
public:
  EventManager();
  ~EventManager() = default;

  void checkEvents(SDL_Event &event, Engine &engine);

private:
  UIManager &uiManager = UIManager::Instance();
  bool handleUIEvents(SDL_Event &event);

  bool isHandlingMouseEvents = false;
};

#endif