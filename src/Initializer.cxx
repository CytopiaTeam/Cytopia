#include "Initializer.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"

Initializer::Initializer()
{
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
  {
    throw CytopiaError{TRACE_INFO "Failed to initialize SDL: " + std::string(SDL_GetError())};
  }

  if (TTF_Init() == -1)
  {
    throw CytopiaError{TRACE_INFO "Failed to initialize SDL_TTF: " + std::string(TTF_GetError())};
  }
}

Initializer::~Initializer() {
  TTF_Quit();
  SDL_Quit();
}
