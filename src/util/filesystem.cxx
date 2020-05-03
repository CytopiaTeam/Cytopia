#include "filesystem.hxx"

std::string fs::getBasePath() noexcept
{
  char *path = SDL_GetBasePath();
  std::string spath{path};
  SDL_free(path);
  return spath;
}
