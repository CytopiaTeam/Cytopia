#include "PixelBuffer.hxx"
#include <SDL2/SDL_image.h>
#include <png.h>
#include <cstdio>

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

PixelBuffer & PixelBuffer::crop(Rectangle && region)
{
  int oWidth = m_Bounds.width();
  m_Bounds.intersect(region);
  int x1 = m_Bounds.x1();
  int x2 = m_Bounds.x2();
  int y1 = m_Bounds.y1();
  int y2 = m_Bounds.y2();
  int width = m_Bounds.width();
  if(m_Bounds.isEmpty())
  {
    m_Pixels.clear();
    return *this;
  }
  std::vector<uint32_t> pixels(width * m_Bounds.height());
  for(int i = x1; i <= x2; ++i)
  {
    for(int j = y1; j <= y2; ++j)
    {
      pixels[(i - x1) + width * (j - y1)] = m_Pixels[i + oWidth * j];
    }
  }
  std::swap(m_Pixels, pixels);
  return *this;
}

PixelBuffer & PixelBuffer::expandCenter(Rectangle && target)
{
  if(m_Bounds.width() >= target.width() ||
     m_Bounds.height() >= target.height())
  {
    throw CytopiaError{TRACE_INFO "Target region is smaller than current bounds"};
  }
  if(isEmpty() || target.isEmpty())
  {
    throw CytopiaError{TRACE_INFO "Cannot expand an empty PixelBuffer or target"};
  }
  uint32_t ow = m_Bounds.width();
  uint32_t oh = m_Bounds.height();
  uint32_t dw = target.width();
  uint32_t dh = target.height();

  std::vector<uint32_t> rescaled(target.width() * target.height(), 0x0);

  uint32_t min_x = ow / 2;
  uint32_t max_x = dw - min_x - (ow % 2);
  uint32_t min_y = oh / 2;
  uint32_t max_y = dh - min_y - (oh % 2);
  
  for(size_t i = 0; i < dw * dh; ++i)
  {
    uint32_t x = i % dw;
    uint32_t y = i / dw;
    if(x >= min_x && x < max_x)
      x = min_x;
    else if(x >= max_x)
    {
      x -= max_x;
      x += min_x;
    }
    if(y >= min_y && y < max_y)
      y = min_y;
    else if(y >= max_y)
    {
      y -= max_y;
      y += min_y;
    }
    rescaled[i] = m_Pixels[x + y * ow];
  }
  std::swap(target, m_Bounds);
  std::swap(rescaled, m_Pixels);
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

  fs::path fpath(GLOBAL_CONTEXT.BASE_PATH / "data");
  error_code errc;
  fpath = boost::filesystem::canonical(fs::path(rpath), fpath, errc);
  if(errc) {
    throw AssetError{TRACE_INFO "Cannot fetch assets which are not in the /data directory: " + rpath};
  }

  FILE* fp = fopen(fpath.c_str(), "rb");
  if(!fp)
    throw AssetError{TRACE_INFO "Could not load asset: " + fpath.generic_string()};
  
  /* Check for PNG */
  unsigned char header[8];
  fread(header, 1, 8, fp);
  if(png_sig_cmp(header, 0, 8)) {
    fclose(fp);
    throw AssetError{TRACE_INFO "Tried to load a non-PNG asset: " + fpath.generic_string()};
  }

  /* Create png object */
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
      nullptr, nullptr, nullptr);
  if(!png) {
    fclose(fp);
    throw AssetError{TRACE_INFO "Could not allocate PNG object: " + fpath.generic_string()};
  }

  png_infop info = png_create_info_struct(png);
  if(!info) {
    fclose(fp);
    png_destroy_read_struct(&png, nullptr, nullptr);
    throw AssetError{TRACE_INFO "Could not allocate PNG info object: " + fpath.generic_string()};
  }

  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);
  png_read_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  int depth = png_get_bit_depth(png, info);
  int channels = png_get_channels(png, info);
  if(depth != 8 or channels != 4) {
    fclose(fp);
    png_destroy_read_struct(&png, &info, nullptr);
    throw AssetError{TRACE_INFO "Unsupported PNG asset with depth = " 
      + std::to_string(depth) + " and channels = " + std::to_string(channels)};
  }
  std::vector<unsigned char> pixels(width * height * channels, 0);
  unsigned char ** rows = png_get_rows(png, info);
  int rowBytes = width * channels;
  for(int i = 0; i < height * rowBytes; ++i)
    pixels[i] = rows[i / rowBytes][i % rowBytes];
  fclose(fp);
  png_destroy_read_struct(&png, &info, nullptr);
  return {0, 0, width, height, std::move(pixels)};
}

PixelBuffer PixelBuffer::EMPTY()
{
  return PixelBuffer { Rectangle::EMPTY(), Pixels(Pixels::iterator(), Pixels::iterator()) };
}
