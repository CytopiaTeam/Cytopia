#include "PixelBuffer.hxx"

PixelBuffer::PixelBuffer(const Rectangle &&r) : m_Bounds(r), m_Pixels(static_cast<std::size_t>(r.width()) * r.height(), 0x0) {}

PixelBuffer::Pixels PixelBuffer::pixels() const { return Pixels{m_Pixels}; }

uint32_t *PixelBuffer::data() noexcept { return m_Pixels.data(); }

const Rectangle &PixelBuffer::bounds() const { return m_Bounds; }

PixelBuffer &PixelBuffer::scale(float factor)
{
  const int original_width = m_Bounds.width();
  const int original_height = m_Bounds.height();
  const int dest_width = original_width * factor;
  const int dest_height = original_height * factor;
  if (dest_width <= 0 || original_height <= 0)
  {
    m_Bounds = Rectangle::EMPTY();
    m_Pixels.clear();
    return *this;
  }
  std::size_t n_pixels = dest_width; // To prevent overflow
  n_pixels *= dest_height;
  std::vector<uint32_t> scaled(n_pixels, 0x0);
  for (std::size_t i = 0; i < n_pixels; ++i)
  {
    std::size_t x = (i % dest_width) / factor;
    std::size_t y = (i / dest_width) / factor;
    scaled[i] = m_Pixels[x + y * original_width];
  }
  std::swap(scaled, m_Pixels);
  m_Bounds = Rectangle{m_Bounds.x1(), m_Bounds.y1(), m_Bounds.x1() + dest_width - 1, m_Bounds.y1() + dest_height - 1};
  return *this;
}

PixelBuffer &PixelBuffer::crop(const Rectangle &region)
{
  const int original_width = m_Bounds.width();
  m_Bounds.intersect(region);
  const int x1 = m_Bounds.x1();
  const int x2 = m_Bounds.x2();
  const int y1 = m_Bounds.y1();
  const int y2 = m_Bounds.y2();
  const int destination_width = m_Bounds.width();
  if (m_Bounds.isEmpty())
  {
    m_Pixels.clear();
    return *this;
  }
  std::size_t n_pixels = original_width;
  n_pixels *= m_Bounds.height();
  std::vector<uint32_t> pixels(n_pixels, 0x0);
  for (int i = x1; i <= x2; ++i)
  {
    for (int j = y1; j <= y2; ++j)
    {
      pixels[(i - x1) + destination_width * (j - y1)] = m_Pixels[i + original_width * j];
    }
  }
  std::swap(m_Pixels, pixels);
  return *this;
}

PixelBuffer &PixelBuffer::expandCenter(Rectangle &&target)
{
  if (m_Bounds.width() >= target.width() || m_Bounds.height() >= target.height())
  {
    throw CytopiaError{TRACE_INFO "Target region is smaller than current bounds"};
  }
  if (isEmpty() || target.isEmpty())
  {
    throw CytopiaError{TRACE_INFO "Cannot expand an empty PixelBuffer or target"};
  }
  const uint32_t original_width = m_Bounds.width();
  const uint32_t original_height = m_Bounds.height();
  const uint32_t destination_width = target.width();
  const uint32_t destination_height = target.height();

  std::size_t n_pixels = target.width();
  n_pixels *= target.height();
  std::vector<uint32_t> rescaled(n_pixels, 0x0);

  const uint32_t min_x = original_width / 2;
  const uint32_t max_x = destination_width - min_x - (original_width % 2);
  const uint32_t min_y = original_height / 2;
  const uint32_t max_y = destination_height - min_y - (original_height % 2);

  for (size_t i = 0; i < n_pixels; ++i)
  {
    uint32_t x = i % destination_width;
    uint32_t y = i / destination_width;
    if (x >= min_x && x < max_x)
    {
      x = min_x;
    }
    else if (x >= max_x)
    {
      x -= max_x;
      x += min_x;
    }
    if (y >= min_y && y < max_y)
    {
      y = min_y;
    }
    else if (y >= max_y)
    {
      y -= max_y;
      y += min_y;
    }
    rescaled[i] = m_Pixels[x + y * original_width];
  }
  std::swap(target, m_Bounds);
  std::swap(rescaled, m_Pixels);
  return *this;
}

PixelBuffer &PixelBuffer::colorMagicPixels(RGBAColor color)
{
  const float h_t = color.hue();
  const float s_t = color.saturation();
  const float l_t = color.lightness();
  for (auto &pixel : m_Pixels)
  {
    uint8_t r = pixel >> 24, b = pixel >> 8, a = pixel;
    if (r != b)
    {
      continue;
    }
    RGBAColor magic{pixel};
    float s_n = overlay(s_t, magic.saturation());
    float l_n = overlay(l_t, magic.lightness());
    pixel = RGBAColor::fromHSLA(h_t, s_n, l_n, a);
  }
  return *this;
}

bool PixelBuffer::isEmpty() const noexcept { return m_Bounds.isEmpty() || m_Pixels.size() == 0; }

float PixelBuffer::overlay(float x, float y)
{
  if (x < 0.5)
  {
    return 2.f * x * y;
  }
  return 1.f - 2.f * (1.f - x) * (1.f - y);
}

PixelBuffer PixelBuffer::EMPTY() { return PixelBuffer{Rectangle::EMPTY(), Pixels(Pixels::iterator(), Pixels::iterator())}; }

SDL_Surface *PixelBuffer::toSurface() noexcept
{
  return SDL_CreateRGBSurfaceFrom(m_Pixels.data(), bounds().width(), bounds().height(), 32, 4 * bounds().width(), 0xFF000000U,
                                  0x00FF0000U, 0x0000FF00U, 0x000000FFU);
}

void PixelBuffer::translateX(int x) noexcept { m_Bounds.translateX(x); }

void PixelBuffer::translateY(int y) noexcept { m_Bounds.translateY(y); }
