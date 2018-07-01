#pragma once

#include <string>
#include <iostream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#include "sprite.hxx"
#include "basics/resources.hxx"
#include "basics/settings.hxx"

class WindowManager
{
public:
  WindowManager() = default;
  explicit WindowManager(const std::string &title);
  ~WindowManager();

  inline bool isRunning() const { return _running; }
  void close() { _running = false; }

  void toggleFullScreen();

private:
  bool init();

private:
  std::string _title;
  int _width = 800;
  int _height = 600;

  bool _running = true;

  SDL_Window *_window = nullptr;
  SDL_Renderer *_renderer = nullptr;
};
