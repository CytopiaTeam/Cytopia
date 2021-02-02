#include "WindowManager.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "basics/Settings.hxx"
#include "Filesystem.hxx"
#include "../renderer/SDLRenderer.hxx"
#include "../view/Window.hxx"
#include <SDL_image.h>

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
  debug_scope {
    LOG(LOG_DEBUG) << "Destroying WindowManager";
  }
}

void WindowManager::toggleFullScreen() const
{
  Settings::instance().fullScreen = !Settings::instance().fullScreen;

  if (Settings::instance().fullScreen)
  {
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(m_window, 0);
  }
}

void WindowManager::setRealWindow(Window & window)
{
  m_window = window.m_Window;
  m_renderer = static_cast<SDLRenderer&>(*window.m_Renderer).m_Renderer;
}

void WindowManager::setWindowTitle(const std::string &title)
{
  m_title = title;
  SDL_SetWindowTitle(m_window, m_title.c_str());
}

