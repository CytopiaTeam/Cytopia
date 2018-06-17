#include "windowManager.hxx"

WindowManager::WindowManager(const std::string &title) : _title(title)
{
  _width = Resources::settings.screenWidth;
  _height = Resources::settings.screenHeight;
  _running = init();
}

WindowManager::~WindowManager()
{
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  TTF_Quit();
  SDL_Quit();
}

bool WindowManager::init()
{
  // Initialize SDL
  if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
    return false;
  }

  // Initialize SDL_TTF
  if (TTF_Init() == -1)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL_TTF\nSDL Error:" << TTF_GetError();
    return false;
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
    return false;
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