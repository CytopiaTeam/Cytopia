#include "windowManager.hxx"

#include "basics/log.hxx"
#include "basics/settings.hxx"

#include <SDL_image.h>

WindowManager::WindowManager()
{
  _width = Settings::instance().settings.screenWidth;
  _height = Settings::instance().settings.screenHeight;

  _window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, 0);
  if (_window == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
  }

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  if (_renderer == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to create Renderer!\nSDL Error:" << SDL_GetError();
  }

  SDL_Surface *icon = IMG_Load(windowIcon.c_str());

  if (icon)
  {
    SDL_SetWindowIcon(_window, icon);
    SDL_FreeSurface(icon);
  }
  else
  {
    LOG(LOG_ERROR) << "Could not load Texture from file " << windowIcon << "\nSDL_IMAGE Error: " << IMG_GetError();
  }
}

WindowManager::~WindowManager()
{
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
}

void WindowManager::toggleFullScreen()
{
  Settings::instance().settings.fullScreen = !Settings::instance().settings.fullScreen;

  if (Settings::instance().settings.fullScreen)
  {
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(_window, 0);
  }
}

void WindowManager::setWindowTitle(const std::string &title)
{
  _title = title;
  SDL_SetWindowTitle(_window, _title.c_str());
}