#include "Canvas.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"
#include "../view/AssetHelper.hxx"

Canvas::Canvas(const Rectangle & r) :
  m_Buffer(r.width() * r.height(), 0), 
  m_Bounds(r)
{
  std::string font = getBasePath();
  font += "resources/fonts/arcadeclassics.ttf";
  m_Font = TTF_OpenFont(font.c_str(), 24);
  if(!m_Font)
    throw CytopiaError{TRACE_INFO "Failed to open font: " 
      + string{TTF_GetError()}};
}

void Canvas::drawText(
    const char * text, 
    RGBAColor color, 
    const Rectangle & bounds, 
    PositionType psType)
{
  SDL_Surface* surface = TTF_RenderUTF8_Blended(m_Font, text, color.to_SDL());
  Rectangle rect { bounds };
  switch(psType)
  {
    case PositionType::Centered:
      rect.translateX((bounds.width() - surface->w) / 2);
      rect.translateY((bounds.height() - surface->h) / 2);
      break;
    case PositionType::Normal:
      break;
  }
  std::vector<uint32_t> pixels(surface->w * surface->h, 0);
  AssetHelper::GetPixelsFromSurface(surface, pixels.data());
  AssetHelper::Render(pixels.data(), m_Buffer.data(), rect, m_Bounds);
  SDL_FreeSurface(surface);
}

void Canvas::drawPicture(const Rectangle &, uint32_t*)
{
  throw UnimplementedError(TRACE_INFO "not implemented");
}

void Canvas::drawShape(const Rectangle &, RGBAColor)
{
  throw UnimplementedError(TRACE_INFO "not implemented");
}

void Canvas::drawLine()
{
  throw UnimplementedError(TRACE_INFO "not implemented");
}

Rectangle Canvas::getDrawableSize() const noexcept
{
  return m_Bounds;
}

Canvas::BufferRange Canvas::getBuffer()
{
  return { m_Buffer.begin(), m_Buffer.end() };
}

uint32_t * Canvas::getData()
{
  return m_Buffer.data();
}

void Canvas::commit()
{
}

void Canvas::clear()
{
  for(std::size_t i = 0; i < m_Buffer.size(); ++i)
  {
    m_Buffer[i] = 0;
  }
}

