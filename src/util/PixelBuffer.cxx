#include "PixelBuffer.hxx"
#include <SDL2/SDL_image.h>

PixelBuffer::PixelBuffer(const Rectangle && r) :
  m_Bounds(r),
  m_Pixels(r.width() * r.height(), 0x0)
{
}

PixelBuffer::Pixels PixelBuffer::pixels() const
{
  return Pixels{ m_Pixels };
}


const Rectangle & PixelBuffer::bounds() const
{
  return m_Bounds;
}


PixelBuffer & PixelBuffer::scale(float factor)
{
  int ow = m_Bounds.width();
  int oh = m_Bounds.height();
  int dw = ow * factor;
  int dh = oh * factor;
  if(dw <= 0 || oh <= 0)
  {
    m_Bounds = Rectangle::EMPTY();
    m_Pixels.clear();
    return *this;
  }
  std::vector<uint32_t> scaled(dw * dh, 0x0);
  for(int i = 0; i < dw * dh; ++i)
  {
    int x = (i % dw) / factor;
    int y = (i / dw) / factor;
    scaled[i] = m_Pixels[x + y * ow];
  }
  std::swap(scaled, m_Pixels);
  m_Bounds = Rectangle {
    m_Bounds.x1(), 
    m_Bounds.y1(),
    m_Bounds.x1() + dw - 1, 
    m_Bounds.y1() + dh - 1
  };
  return *this;
}


PixelBuffer & PixelBuffer::colorMagicPixels(RGBAColor color)
{
  const float h_t = color.hue();
  const float s_t = color.saturation();
  const float l_t = color.lightness();
  
  for(auto & pixel : m_Pixels)
  {
    uint8_t r = pixel >> 24,
            g = pixel >> 16,
            b = pixel >> 8,
            a = pixel;
    if(r != b) continue;
    RGBAColor magic {r, g, b, a};
    float s_n = overlay(magic.saturation(), s_t);
    float l_n = overlay(magic.lightness(), l_t);
    pixel = RGBAColor::fromHSLA(h_t, s_n, l_n, a);
  }
  return *this;
}

bool PixelBuffer::isEmpty() const noexcept
{
  return m_Bounds.isEmpty() || m_Pixels.size() == 0;
}

float PixelBuffer::overlay(float x, float y)
{
  if(x < 0.5)
  {
    return 2.f * x * y;
  }
  return 1.f - 2.f * (1.f - x) * (1.f - y);
}

PixelBuffer PixelBuffer::fromPNG(std::string filepath)
{
  auto surface = IMG_Load(filepath.c_str());
  if(!surface)
  {
    LOG(LOG_WARNING) << TRACE_INFO "Couldn't load image pixels: " << IMG_GetError();
    return EMPTY();
  }
  auto w = surface->w, h = surface->h;
  SDL_LockSurface(surface);
  auto format = surface->format;
  uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
  std::vector<uint32_t> data(w * h, 0x0);
  for(std::size_t i = 0; i < w * h; ++i)
  {
    data[i] = 
      (pixels[i] & format->Rmask) >> format->Rshift << 24 |
      (pixels[i] & format->Gmask) >> format->Gshift << 16 |
      (pixels[i] & format->Bmask) >> format->Bshift << 8  |
      (pixels[i] & format->Amask) >> format->Ashift << 0;
  }
  SDL_UnlockSurface(surface);
  return { Rectangle{0, 0, w - 1, h - 1}, std::move(data) };
}

PixelBuffer PixelBuffer::EMPTY()
{
  return PixelBuffer { Rectangle::EMPTY() };
}
