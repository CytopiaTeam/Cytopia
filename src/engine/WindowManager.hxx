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

  /** \brief sets title of the window
  * Sets title of the game window
  * @param title the new title of the window
  */
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
  void setLastScreenResolution() { setScreenResolution(m_lastSelectedResolutionIdx); };

  /** \brief sets the game's window mode
  * Changes the game's window mode to fullscreen, borderless, or windowed
  * @param mode the new window mode, WINDOWED, BORDERLESS, or FULLSCREEN
  */
  void setFullScreenMode(FULLSCREEN_MODE mode) const;

private:
  /**
  * \brief Construct a new WindowManager object.
  */
  WindowManager();
  /**
  * \brief Destroy the WindowManager object.
  */
  ~WindowManager();

  std::string m_title = "Cytopia";                                                /// title of the window
  const std::string m_windowIcon = "resources/images/app_icons/cytopia_icon.png"; /// the window's icon

  SDL_Window *m_window = nullptr;     /// pointer to the SDL_Window
  SDL_Renderer *m_renderer = nullptr; /// pointer to the SDL_Renderer

  std::vector<SDL_DisplayMode *> m_resolutions;

  int m_numOfDisplays = 0;
  int m_activeDisplay = 0;
  int m_lastSelectedResolutionIdx = 0;

  void initializeScreenResolutions();
};
