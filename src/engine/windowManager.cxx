#include "windowManager.hxx"

WindowManager::WindowManager(const std::string &title) : _title()
{
  _width = Resources::settings.screenWidth;
  _height = Resources::settings.screenHeight;
  _closed = !init();
}

WindowManager::~WindowManager()
{
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

bool WindowManager::init()
{
  if ( SDL_Init(SDL_INIT_VIDEO != 0) )
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
    return 0;
  }
  
  _window = SDL_CreateWindow(_title.c_str(),
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             _width, _height,
                             0
                            );
  
  if ( _window == nullptr )
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
    return 0;
  }

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

  if ( _renderer == nullptr )
  {
	  LOG(LOG_ERROR) << "Failed to create Renderer!\nSDL Error:" << SDL_GetError();
  }

  Resources::setWindow(_window);
  Resources::setRenderer(_renderer);

  return true;
}

void WindowManager::toggleFullScreen()
{
  Resources::settings.fullScreen = !Resources::settings.fullScreen;

  if (Resources::settings.fullScreen)
  {
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(_window, 0);
  }
}