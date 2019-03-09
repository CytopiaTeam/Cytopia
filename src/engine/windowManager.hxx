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

  void toggleFullScreen() const;

  void setWindowTitle(const std::string &title);

  /** \brief get SDL Renderer
  * Returns a pointer to the SDL Renderer
  * \return Pointer to the SDL_Renderer
  */
  SDL_Renderer *getRenderer() const { return m_renderer; };

  /** \brief get SDL Window
  * Returns a pointer to the SDL_Window
  * \return Pointer to the SDL_Window
  */
  SDL_Window *getWindow() const { return m_window; };

private:
  WindowManager();
  ~WindowManager();

  std::string m_title = "Cytopia";
  const std::string m_windowIcon = "resources/images/app_icons/cytopia_icon.png";

  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
};
