#include "windowManager.hxx"

#include "basics/log.hxx"
#include "basics/settings.hxx"

#include <SDL_image.h>

WindowManager::WindowManager()
{
  m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Settings::instance().settings.screenWidth, Settings::instance().settings.screenHeight, 0);
  if (m_window == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
  }

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to create Renderer!\nSDL Error:" << SDL_GetError();
  }

  SDL_Surface *icon = IMG_Load(m_windowIcon.c_str());

  if (icon)
  {
    SDL_SetWindowIcon(m_window, icon);
    SDL_FreeSurface(icon);
  }
  else
  {
    LOG(LOG_ERROR) << "Could not load Texture from file " << m_windowIcon << "\nSDL_IMAGE Error: " << IMG_GetError();
  }
}

WindowManager::~WindowManager()
{
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
}

void WindowManager::toggleFullScreen() const
{
  Settings::instance().settings.fullScreen = !Settings::instance().settings.fullScreen;

  if (Settings::instance().settings.fullScreen)
  {
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(m_window, 0);
  }
}

void WindowManager::setWindowTitle(const std::string &title)
{
  m_title = title;
  SDL_SetWindowTitle(m_window, m_title.c_str());
}