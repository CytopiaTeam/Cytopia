#include "windowManager.hxx"

WindowManager::WindowManager(std::string title) : _title(std::move(title))
{
  _width = Settings::Instance().settings.screenWidth;
  _height = Settings::Instance().settings.screenHeight;
  _running = init();
}

WindowManager::~WindowManager()
{
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
}

bool WindowManager::init()
{
  _window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, 0);
  if (_window == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
    return false;
  }

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  if (_renderer == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to create Renderer!\nSDL Error:" << SDL_GetError();
  }

  Resources::setWindow(_window);
  Resources::setRenderer(_renderer);

  return true;
}

void WindowManager::toggleFullScreen()
{
  Settings::Instance().settings.fullScreen = !Settings::Instance().settings.fullScreen;

  if (Settings::Instance().settings.fullScreen)
  {
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(_window, 0);
  }
}