#include "window.hxx"

Window::Window(const std::string &title, int width, int height) :
_title(title), _width(width), _height(height)
{
  _closed = !init();
  _isFullScreen = false;

}

Window::~Window()
{
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
  
}

bool Window::init()
{
  if ( SDL_Init(SDL_INIT_VIDEO != 0) )
  {
    printf("Failed to Init SDL");
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
    printf("Failed to Init SDL");
    return 0;
  }

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

  if ( _renderer == nullptr )
  {
	  printf("Failed to create Renderer!");
  }

  Resources::setWindow(_window);
  Resources::setRenderer(_renderer);

  return true;
}

void Window::toggleFullScreen()
{
  _isFullScreen = !_isFullScreen;

  if (_isFullScreen)
  {
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(_window, 0);
  }
}