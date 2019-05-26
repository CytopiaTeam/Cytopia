#include "sprite.hxx"

#include "resourcesManager.hxx"
#include "windowManager.hxx"
#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "map/MapLayers.hxx"

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

Sprite::Sprite(Point _isoCoordinates) : isoCoordinates(_isoCoordinates)
{
  m_screenCoordinates = convertIsoToScreenCoordinates(_isoCoordinates);
  m_SpriteData.resize(LAYERS_COUNT); // resize the spritedata vector to the amount of layers we have.
}

void Sprite::render() const
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Sprite render", MP_RED);
#endif
  for (uint32_t i = 0; i < LAYERS_COUNT; ++i)
  {
    if (MapLayers::isLayerActive(i))
    {
      if (m_SpriteData[i].texture)
      {
        if (highlightSprite)
        {
          SDL_SetTextureColorMod(m_SpriteData[i].texture, 150, 150, 150);
        }

        if (m_SpriteData[i].clipRect.w != 0)
        {
          SDL_RenderCopy(WindowManager::instance().getRenderer(), m_SpriteData[i].texture, &m_SpriteData[i].clipRect,
                         &m_SpriteData[i].destRect);
        }
        else
        {
          SDL_RenderCopy(WindowManager::instance().getRenderer(), m_SpriteData[i].texture, nullptr, &m_SpriteData[i].destRect);
        }

        if (highlightSprite)
        {
          SDL_SetTextureColorMod(m_SpriteData[i].texture, 255, 255, 255);
        }
      }
    }
  }
}

void Sprite::refresh()
{
  if (m_currentZoomLevel != Camera::zoomLevel || m_needsRefresh)
  {
    for (uint32_t it = 0; it < LAYERS_COUNT; ++it)
    {
      m_currentZoomLevel = Camera::zoomLevel;
      if (m_SpriteData[it].clipRect.w != 0)
      {
        m_SpriteData[it].destRect.w = static_cast<int>(m_SpriteData[it].clipRect.w * m_currentZoomLevel);
        m_SpriteData[it].destRect.h = static_cast<int>(m_SpriteData[it].clipRect.h * m_currentZoomLevel);
      }
      else
      {
        SDL_QueryTexture(m_SpriteData[it].texture, nullptr, nullptr, &m_SpriteData[it].destRect.w, &m_SpriteData[it].destRect.h);
        m_SpriteData[it].destRect.w = static_cast<int>(m_SpriteData[it].destRect.w * m_currentZoomLevel);
        m_SpriteData[it].destRect.h = static_cast<int>(m_SpriteData[it].destRect.h * m_currentZoomLevel);
      }
    }
  }

  m_screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates);

  for (uint32_t it = 0; it < LAYERS_COUNT; ++it)
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
  m_SpriteData[layer].texture = texture;
  m_needsRefresh = true;
  refresh();
}

void Sprite::setClipRect(SDL_Rect clipRect, const Layer layer) { m_SpriteData[layer].clipRect = clipRect; }
void Sprite::setDestRect(SDL_Rect destRect, Layer layer) { m_SpriteData[layer].destRect = destRect; }

SDL_Rect Sprite::getActiveClipRect()
{
  if (MapLayers::isLayerActive(Layer::BUILDINGS) && m_SpriteData[Layer::BUILDINGS].clipRect.w != 0 &&
      m_SpriteData[Layer::BUILDINGS].clipRect.h != 0)
  {
    return m_SpriteData[Layer::BUILDINGS].clipRect;
  }
  else if (MapLayers::isLayerActive(Layer::TERRAIN))
  {
    return m_SpriteData[Layer::TERRAIN].clipRect;
  }
  return {0, 0, 0, 0};
}
bool Sprite::isLayerUsed(Layer layer)
{
  return (MapLayers::isLayerActive(layer) && m_SpriteData[layer].clipRect.w != 0 && m_SpriteData[layer].clipRect.h != 0);
}

SDL_Rect Sprite::getActiveDestRect()
{
  if (MapLayers::isLayerActive(Layer::BUILDINGS) && m_SpriteData[Layer::BUILDINGS].destRect.w != 0 &&
      m_SpriteData[Layer::BUILDINGS].destRect.h != 0)
  {
    return m_SpriteData[Layer::BUILDINGS].destRect;
  }
  else if (MapLayers::isLayerActive(Layer::TERRAIN))
  {
    return m_SpriteData[Layer::TERRAIN].destRect;
  }
  return {0, 0, 0, 0};
}

void Sprite::clearSprite(Layer layer)
{
  m_SpriteData[layer].clipRect = {0, 0, 0, 0};
  m_SpriteData[layer].destRect = {0, 0, 0, 0};
  m_SpriteData[layer].texture = nullptr;
}
