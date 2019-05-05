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
  m_destRects[Layer::TERRAIN] = SDL_Rect{0, 0, 0, 0};
  m_destRects[Layer::BUILDINGS] = SDL_Rect{0, 0, 0, 0};
  m_clipRects[Layer::TERRAIN] = SDL_Rect{0, 0, 0, 0};
  m_clipRects[Layer::BUILDINGS] = SDL_Rect{0, 0, 0, 0};
}

void Sprite::render() const
{
  for (const int &it : MapLayers::getActiveLayers())
  {
    if (m_textures.count(it))
    {
      if (highlightSprite == true)
      {
        SDL_SetTextureColorMod(m_textures.at(it), 150, 150, 150);
      }

      if (clipRect.w != 0)
      {
        SDL_RenderCopy(WindowManager::instance().getRenderer(), m_textures.at(it), &m_clipRects.at(it), &m_destRects.at(it));
      }
      else
      {
        SDL_RenderCopy(WindowManager::instance().getRenderer(), m_textures.at(it), nullptr, &m_destRects.at(it));
      }

      if (highlightSprite == true)
      {
        SDL_SetTextureColorMod(m_textures.at(it), 255, 255, 255);
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
      if (m_clipRects.at(it).w != 0)
      {
        m_destRects[it].w = 32;
        m_destRects.at(it).w = static_cast<int>(m_clipRects.at(it).w * m_currentZoomLevel);
        m_destRects.at(it).h = static_cast<int>(m_clipRects.at(it).h * m_currentZoomLevel);
      }
      else
      {
        SDL_QueryTexture(m_texture, nullptr, nullptr, &m_destRects.at(it).w, &m_destRects.at(it).h);
        m_destRects.at(it).w = static_cast<int>(m_destRects.at(it).w * m_currentZoomLevel);
        m_destRects.at(it).h = static_cast<int>(m_destRects.at(it).h * m_currentZoomLevel);
      }
    }
  }

  m_screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates);

  for (const int &it : MapLayers::getActiveLayers())
  {
    // render the sprite in the middle of its bounding box so bigger than 1x1 sprites will render correctly
    m_destRects.at(it).x = m_screenCoordinates.x - (m_destRects.at(it).w / 2);
    // change y coordinates with sprites height taken into account to render the sprite at its base and not at its top.
    m_destRects.at(it).y = m_screenCoordinates.y - m_destRects.at(it).h;
  }
  m_needsRefresh = false;
}

void Sprite::setTexture(SDL_Texture *texture, Layer layer)
{
  m_textures[layer] = texture;
  m_destRects[layer] = SDL_Rect{0, 0, 0, 0};
  m_needsRefresh = true;
  refresh();
}

void Sprite::setClipRect(SDL_Rect aclipRect, Layer layer) { m_clipRects[layer] = aclipRect; }
void Sprite::setDestRect(SDL_Rect aDestRect, Layer layer) { m_destRects[layer] = aDestRect; }