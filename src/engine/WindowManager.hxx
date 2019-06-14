#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <SDL.h>

#include "../util/Singleton.hxx"

enum class FULLSCREEN_MODE
{
  WINDOWED = 0,
  BORDERLESS = 1,
  FULLSCREEN = 2
};

class WindowManager : public Singleton<WindowManager>
{
public:
  friend Singleton<WindowManager>;

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

  std::vector<SDL_DisplayMode *> getSupportedScreenResolutions() { return m_resolutions; };

  void setScreenResolution(int mode);

  void setFullScreenMode(FULLSCREEN_MODE mode) const;

private:
  WindowManager();
  ~WindowManager();

  std::string m_title = "Cytopia";
  const std::string m_windowIcon = "resources/images/app_icons/cytopia_icon.png";

  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  void initializeScreenResolutions();

  std::vector<SDL_DisplayMode *> m_resolutions;

  int m_numOfDisplays = 0;
  int m_activeDisplay = 0;
};
