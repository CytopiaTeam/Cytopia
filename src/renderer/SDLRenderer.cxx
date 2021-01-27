#include "SDLRenderer.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"
#include "../util/Filesystem.hxx"
#include "../engine/basics/Settings.hxx"

SDLRenderer::SDLRenderer(SDL_Window *sdl_window)
{
  //TODO: Find proper renderer creation
  m_Renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);

  if (!m_Renderer)
    throw UIError(TRACE_INFO "Failed to create Renderer: " + string{SDL_GetError()});

  // If an accelerated renderer is not possible, an error is logged even if
  // we get a software renderer
  SDL_ClearError();

  if (SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND) != 0)
    throw CytopiaError{TRACE_INFO "Failed to enable Alpha" + string{SDL_GetError()}};

  std::string font = fs::getBasePath();
  font += Settings::instance().fontFileName.get();
  m_Font = TTF_OpenFont(font.c_str(), 16);
  if (!m_Font)
    throw CytopiaError{TRACE_INFO "Failed to open font: " + string{TTF_GetError()}};

  if (SDL_RenderClear(m_Renderer) != 0)
    throw CytopiaError{TRACE_INFO "Failed to clear canvas" + string{SDL_GetError()}};
  SDL_RenderPresent(m_Renderer);

  const char *error = SDL_GetError();
  if (*error)
  {
    LOG(LOG_ERROR) << TRACE_INFO "SDL Error: " << error;
    SDL_ClearError();
  }
}

SDLRenderer::~SDLRenderer()
{
  LOG(LOG_WARNING) << "Destroying my renderer... Printing all errors which have happened";

  const char *error = SDL_GetError();
  if (*error)
  {
    LOG(LOG_ERROR) << "SDL Error: " << error;
  }
  SDL_ClearError();
  SDL_DestroyRenderer(m_Renderer);
}

void SDLRenderer::drawText(const char *text, RGBAColor color, const Rectangle &position, PositionType psType)
{
  SDL_Surface *surface = TTF_RenderUTF8_Blended(m_Font, text, color.to_SDL());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
  SDL_Rect rect = position.to_SDL();
  rect.w = surface->w;
  rect.h = surface->h;
  switch (psType)
  {
  case PositionType::Centered:
    rect.x += (position.width() - surface->w) / 2;
    rect.y += (position.height() - surface->h) / 2;
    break;
  case PositionType::Normal:
    break;
  }
  SDL_RenderCopy(m_Renderer, texture, nullptr, &rect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void SDLRenderer::drawPicture(const Rectangle &rect, uint32_t *data)
{
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, rect.width(), rect.height(), 32, 4 * rect.width(), 0xff000000, 0x00ff0000,
                                                  0x0000ff00, 0x000000ff);
  if (surface == nullptr)
  {
    LOG(LOG_ERROR) << TRACE_INFO "Failed to create an SDL_Surface: " << SDL_GetError();
    SDL_ClearError();
    return;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
  if (texture == nullptr)
  {
    LOG(LOG_ERROR) << TRACE_INFO "Failed to create an SDL_Texture: " << SDL_GetError();
    SDL_ClearError();
    return;
  }
  SDL_Rect position = rect.to_SDL();
  if (SDL_RenderCopy(m_Renderer, texture, nullptr, &position) != 0)
  {
    LOG(LOG_ERROR) << TRACE_INFO "Failed to render:" << SDL_GetError();
    SDL_ClearError();
    return;
  }
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void SDLRenderer::drawShape(const Rectangle &r, RGBAColor c)
{
  auto color = c.to_SDL();
  SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
  SDL_Rect rect = r.to_SDL();
  SDL_RenderFillRect(m_Renderer, &rect);
  // TODO: remove once proper handling of background picture is implemented
  SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
}

void SDLRenderer::drawLine() { throw UnimplementedError{TRACE_INFO "Unimplemented"}; }

Rectangle SDLRenderer::getDrawableSize() const noexcept
{
  int w, h;
  SDL_GetRendererOutputSize(m_Renderer, &w, &h);
  return Rectangle{0, 0, w, h};
}

void SDLRenderer::setViewport(const Rectangle &r)
{
  const SDL_Rect rect = r.to_SDL();
  SDL_RenderSetViewport(m_Renderer, &rect);
}

void SDLRenderer::resetViewport() { SDL_RenderSetViewport(m_Renderer, NULL); }

void SDLRenderer::commit()
{
  SDL_RenderPresent(m_Renderer);
  const char *error = SDL_GetError();
  if (*error)
    LOG(LOG_ERROR) << "SDL Error: " << error;
  SDL_ClearError();
}

void SDLRenderer::clear()
{
  SDL_RenderClear(m_Renderer);
  const char *error = SDL_GetError();
  if (*error)
    LOG(LOG_ERROR) << "SDL Error: " << error;
  SDL_ClearError();
}
