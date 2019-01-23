#pragma once

#include <string>
#include <iostream>

#include <SDL.h>

class WindowManager
{
public:
  /// Retrieves instance of Singleton class WindowManager
  static WindowManager &instance()
  {
    static WindowManager windowManager;
    return windowManager;
  }

  // Disable copy and assignemnt operators
  WindowManager(WindowManager const &) = delete;
  WindowManager &operator=(WindowManager const &) = delete;

  void toggleFullScreen();

  void setWindowTitle(const std::string &title);

  /** \brief get SDL Renderer
  * Returns a pointer to the SDL Renderer
  * \return Pointer to the SDL_Renderer
  */
  SDL_Renderer *getRenderer() { return _renderer; };

  /** \brief get SDL Window
  * Returns a pointer to the SDL_Window
  * \return Pointer to the SDL_Window
  */
  SDL_Window *getWindow() { return _window; };

private:
  WindowManager();
  ~WindowManager();

  std::string _title = "Cytopia";
  const std::string windowIcon = "resources/images/app_icons/cytopia_icon.png";
  int _width = 800;
  int _height = 600;

  SDL_Window *_window = nullptr;
  SDL_Renderer *_renderer = nullptr;
};
