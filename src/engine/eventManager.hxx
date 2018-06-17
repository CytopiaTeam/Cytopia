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


  bool checkEvents(SDL_Event &event);
private:
  UIManager& uiManager = UIManager::Instance();
};

#endif