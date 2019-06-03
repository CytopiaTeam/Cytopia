#include "Sprite.hxx"

#include "ResourcesManager.hxx"
#include "WindowManager.hxx"
#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/LOG.hxx"

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

Sprite::Sprite(Point _isoCoordinates) : isoCoordinates(_isoCoordinates)
{
  m_screenCoordinates = convertIsoToScreenCoordinates(_isoCoordinates);
}

void Sprite::render() const
{
#ifdef MICROPROFILE_ENABLED
    MICROPROFILE_SCOPEI ("Map", "Sprite render", MP_RED);
#endif

  if (highlightSprite == true)
  {
    SDL_SetTextureColorMod(m_texture, 150, 150, 150);
  }

  if (clipRect.w != 0)
  {
    SDL_RenderCopy(WindowManager::instance().getRenderer(), m_texture, &clipRect, &destRect);
  }
  else
  {
    SDL_RenderCopy(WindowManager::instance().getRenderer(), m_texture, nullptr, &destRect);
  }

  if (highlightSprite == true)
  {
    SDL_SetTextureColorMod(m_texture, 255, 255, 255);
  }
}

void Sprite::refresh()
{
  if (m_currentZoomLevel != Camera::zoomLevel || m_needsRefresh)
  {
    m_currentZoomLevel = Camera::zoomLevel;
    if (clipRect.w != 0)
    {
      destRect.w = static_cast<int>(clipRect.w * m_currentZoomLevel);
      destRect.h = static_cast<int>(clipRect.h * m_currentZoomLevel);
    }
    else
    {
      SDL_QueryTexture(m_texture, nullptr, nullptr, &destRect.w, &destRect.h);
      destRect.w = static_cast<int>(destRect.w * m_currentZoomLevel);
      destRect.h = static_cast<int>(destRect.h * m_currentZoomLevel);
    }
  }

  m_screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates);
  // render the sprite in the middle of its bounding box so bigger than 1x1 sprites will render correctly
  destRect.x = m_screenCoordinates.x - (destRect.w / 2);
  // change y coordinates with sprites height taken into account to render the sprite at its base and not at its top.
  destRect.y = m_screenCoordinates.y - destRect.h;
  m_needsRefresh = false;
}

void Sprite::setTexture(SDL_Texture *texture)
{
  m_texture = texture;
  m_needsRefresh = true;
  refresh();
}
