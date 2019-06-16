#include "Image.hxx"

void Image::draw()
{
  if (m_texture)
  {
    SDL_SetTextureAlphaMod(m_texture, m_alphaLevel);
    renderTexture();
    SDL_SetTextureAlphaMod(m_texture, SDL_ALPHA_OPAQUE);
  }
}
