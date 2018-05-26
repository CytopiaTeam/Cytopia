//#include "basics/log.hxx"

#include "SDL2/SDL.h"

#include "uiManager.hxx"

class EventManager
{
public:
  EventManager();
  ~EventManager() = default;


  bool checkEvents(SDL_Event &event);
private:
  UIManager& uiManager = UIManager::Instance();
};