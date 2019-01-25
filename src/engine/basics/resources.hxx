#ifndef RESOURCES_HXX_
#define RESOURCES_HXX_

#include <unordered_map>

#include <SDL.h>

#include "point.hxx"

// Namespace from json library - https://github.com/nlohmann/json

//forward declare Engine class (prevent cross includes)
class Engine;

class Resources
{
public:
private:
  Resources() = default;
  ~Resources() = default;

  static SDL_Renderer *_renderer;
  static SDL_Window *_window;
};

#endif
