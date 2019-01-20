#pragma once

#include <string>
#include <iostream>

#include "SDL2/SDL.h"

class WindowManager
{
public:
  WindowManager() = default;
  explicit WindowManager(std::string title);
  ~WindowManager();

  inline bool isRunning() const { return _running; }
  void close() { _running = false; }

  void toggleFullScreen();

private:
  bool init();

private:
  std::string _title;
  const std::string windowIcon = "resources/images/app_icons/cytopia_icon.png";
  int _width = 800;
  int _height = 600;

  bool _running = true;

  SDL_Window *_window = nullptr;
  SDL_Renderer *_renderer = nullptr;
};
