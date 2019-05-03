#include "sprite.hxx"

#include "resourcesManager.hxx"
#include "windowManager.hxx"
#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/log.hxx"

Sprite::Sprite(Point _isoCoordinates) : isoCoordinates(_isoCoordinates)
{
  m_screenCoordinates = convertIsoToScreenCoordinates(_isoCoordinates);
}

void Sprite::render(const std::vector<Layer> &layers) const
{
  //LOG() << " a " << Layer::_from_index(layers);
  for (const int &it : layers)
  //for (const auto &it : m_textures)
  {
    if (m_textures.count(it))
    {
      if (highlightSprite == true)
      {
        SDL_SetTextureColorMod(m_textures.at(it), 150, 150, 150);
      }

      if (clipRect.w != 0)
      {
        SDL_RenderCopy(WindowManager::instance().getRenderer(), m_textures.at(it), &clipRect, &destRect);
      }
      else
      {
        SDL_RenderCopy(WindowManager::instance().getRenderer(), m_textures.at(it), nullptr, &destRect);
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

void Sprite::setTexture(SDL_Texture *texture, Layer layer)
{
  m_textures[layer] = texture;
  m_needsRefresh = true;
  refresh();
}
