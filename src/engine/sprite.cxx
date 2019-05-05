#include "sprite.hxx"

#include "resourcesManager.hxx"
#include "windowManager.hxx"
#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/log.hxx"
#include "map/MapLayers.hxx"

Sprite::Sprite(Point _isoCoordinates) : isoCoordinates(_isoCoordinates)
{
  m_screenCoordinates = convertIsoToScreenCoordinates(_isoCoordinates);
  m_SpriteData.resize(Layer::_size()); // resize the spritedata layer to the amount of layers we have.
}

void Sprite::render() const
{
  for (const int &it : MapLayers::getActiveLayers())
  {
    if (m_SpriteData[it].texture)
    {
      if (highlightSprite == true)
      {
        SDL_SetTextureColorMod(m_SpriteData[it].texture, 150, 150, 150);
      }

      if (clipRect.w != 0)
      {
        SDL_RenderCopy(WindowManager::instance().getRenderer(), m_SpriteData[it].texture, &m_SpriteData[it].clipRect,
                       &m_SpriteData[it].destRect);
      }
      else
      {
        SDL_RenderCopy(WindowManager::instance().getRenderer(), m_SpriteData[it].texture, nullptr, &m_SpriteData[it].destRect);
      }

      if (highlightSprite == true)
      {
        SDL_SetTextureColorMod(m_SpriteData[it].texture, 255, 255, 255);
      }
    }
  }
}

void Sprite::refresh()
{
  if (m_currentZoomLevel != Camera::zoomLevel || m_needsRefresh)
  {
    for (const int &it : MapLayers::getActiveLayers())
    {
      m_currentZoomLevel = Camera::zoomLevel;
      if (m_SpriteData[it].clipRect.w != 0)
      {
        m_SpriteData[it].destRect.w = static_cast<int>(m_SpriteData[it].clipRect.w * m_currentZoomLevel);
        m_SpriteData[it].destRect.h = static_cast<int>(m_SpriteData[it].clipRect.h * m_currentZoomLevel);
      }
      else
      {
        SDL_QueryTexture(m_texture, nullptr, nullptr, &m_SpriteData[it].destRect.w, &m_SpriteData[it].destRect.h);
        m_SpriteData[it].destRect.w = static_cast<int>(m_SpriteData[it].destRect.w * m_currentZoomLevel);
        m_SpriteData[it].destRect.h = static_cast<int>(m_SpriteData[it].destRect.h * m_currentZoomLevel);
      }
    }
  }

  m_screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates);

  for (const int &it : MapLayers::getActiveLayers())
  {
    // render the sprite in the middle of its bounding box so bigger than 1x1 sprites will render correctly
    m_SpriteData[it].destRect.x = m_screenCoordinates.x - (m_SpriteData[it].destRect.w / 2);
    // change y coordinates with sprites height taken into account to render the sprite at its base and not at its top.
    m_SpriteData[it].destRect.y = m_screenCoordinates.y - m_SpriteData[it].destRect.h;
  }
  m_needsRefresh = false;
}

void Sprite::setTexture(SDL_Texture *texture, Layer layer)
{
  m_SpriteData[layer._to_index()].texture = texture;
  m_needsRefresh = true;
  refresh();
}

void Sprite::setClipRect(SDL_Rect clipRect, Layer layer) { m_SpriteData[layer._to_index()].clipRect = clipRect; }
void Sprite::setDestRect(SDL_Rect destRect, Layer layer) { m_SpriteData[layer._to_index()].destRect = destRect; }