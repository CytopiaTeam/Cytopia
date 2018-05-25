//#include "basics/log.hxx"

#include "SDL2/SDL.h"

class EventManager
{
public:
  EventManager();
  ~EventManager() = default;


  bool checkEvents(SDL_Event &event);


};