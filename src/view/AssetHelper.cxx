#include "AssetHelper.hxx"
#include "../util/LOG.hxx"
  
void AssetHelper::GetPixelsFromSurface(SDL_Surface * surface, uint32_t* data)
{
  SDL_LockSurface(surface);
  auto format = surface->format;
  uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
  for(std::size_t i = 0; i < surface->w * surface->h; ++i)
  {
    *data++ = 
      (pixels[i] & format->Rmask) >> format->Rshift << 24 |
      (pixels[i] & format->Gmask) >> format->Gshift << 16 |
      (pixels[i] & format->Bmask) >> format->Bshift << 8  |
      (pixels[i] & format->Amask) >> format->Ashift << 0;
  }
  SDL_UnlockSurface(surface);
}

void AssetHelper::MakeColRepeatImage(uint32_t* orig, uint32_t* dest, const Rectangle & orig_rect, const Rectangle & dest_rect)
{
  uint32_t ow = orig_rect.width();
  uint32_t oh = orig_rect.height();
  uint32_t dw = dest_rect.width();
  uint32_t dh = dest_rect.height();

  uint32_t y_ratio = (oh - 1) << 16;
  y_ratio /= (dh - 1);
  uint32_t min_x = ow * dh / 2 / oh;
  uint32_t max_x = dw - min_x;
  
  for(size_t i = 0; i < dw * dh; ++i)
  {
    uint32_t x = i % dw;
    x = std::min(
        x,
        std::clamp(x, max_x, dw) - max_x + min_x);
    x *= y_ratio;
    x >>= 16;
    uint32_t y = (i / dw) * y_ratio >> 16;
    dest[i] = orig[x + y * ow];
  }
}

void AssetHelper::ResizeNearest(uint32_t* orig, uint32_t* dest, const Rectangle & orig_rect, const Rectangle & dest_rect)
{
  uint32_t ow = orig_rect.width();
  uint32_t oh = orig_rect.height();
  uint32_t dw = dest_rect.width();
  uint32_t dh = dest_rect.height();

  uint32_t x_ratio = ow << 16;
  x_ratio /= dw;
  uint32_t y_ratio = oh << 16;
  y_ratio /= dh;

  for(size_t i = 0; i < dw * dh; ++i)
  {
    uint32_t x = (i % dw) * x_ratio >> 16;
    uint32_t y = (i / dw) * y_ratio >> 16;
    dest[i] = orig[x + y * ow];
  }
}

void AssetHelper::RepeatXY(uint32_t* orig, uint32_t* dest, const Rectangle & orig_rect, const Rectangle & dest_rect)
{
  uint32_t ow = orig_rect.width();
  uint32_t oh = orig_rect.height();
  uint32_t dw = dest_rect.width();
  uint32_t dh = dest_rect.height();
  
  for(size_t i = 0; i < dw * dh; ++i)
  {
    uint32_t x = i % dw % ow;
    uint32_t y = i / dw % oh;
    dest[i] = orig[x + y * ow];
  }
}

void AssetHelper::NNExpandSprite(uint32_t* from, uint32_t* to, 
    const Rectangle & rfrom, const Rectangle & rto, const Rectangle & corner)
{
  uint32_t ow = rfrom.width();
  uint32_t oh = rfrom.height();
  uint32_t dw = rto.width();
  uint32_t dh = rto.height();
  uint32_t cw = corner.width();
  uint32_t ch = corner.height();

  if(dw * dh == 0)
    return;

  uint32_t x_ratio = cw << 17;
  x_ratio /= ow;
  uint32_t y_ratio = ch << 17;
  y_ratio /= oh;

  Rectangle scaleTo {
    0, 0, 
    static_cast<int>((ow * x_ratio) >> 16),
    static_cast<int>((oh * y_ratio) >> 16)
  };
  std::vector<uint32_t> rescaled(scaleTo.width() * scaleTo.height(), 0x0);
  AssetHelper::ResizeNearest(from, rescaled.data(), rfrom, scaleTo);

  uint32_t min_x = cw;
  uint32_t max_x = dw - min_x;
  uint32_t min_y = ch;
  uint32_t max_y = dh - min_y;
  
  for(size_t i = 0; i < dw * dh; ++i)
  {
    uint32_t x = i % dw;
    uint32_t y = i / dw;
    if(x >= min_x && x <= max_x)
      x = scaleTo.width() / 2;
    else if(x > max_x)
    {
      x -= max_x;
      x += scaleTo.width()/2;
    }
    if(y >= min_y && y <= max_y)
      y = scaleTo.height() / 2;
    else if(y > max_y)
    {
      y -= max_y;
      y += scaleTo.height()/2;
    }
    to[i] = rescaled[x + y * scaleTo.width()];
  }
}

size_t AssetHelper::CropImage(uint32_t* from, uint32_t* to, const Rectangle & rfrom, Rectangle & rto)
{
  auto [fp1x, fp1y] = rfrom.p1();
  auto [fp2x, fp2y] = rfrom.p2();
  auto [tp1x, tp1y] = rto.p1();
  auto [tp2x, tp2y] = rto.p2();
  tp1x = std::clamp(tp1x, fp1x, fp2x);
  tp1y = std::clamp(tp1y, fp1y, fp2y);
  tp2x = std::clamp(tp2x, fp1x, fp2x);
  tp2y = std::clamp(tp2y, fp1y, fp2y);
  size_t w = rfrom.width();
  for(size_t y = tp1y; y <= tp2y; ++y)
  {
    for(size_t x = tp1x; x <= tp2x; ++x)
    {
      *to++ = from[x + y * w];
    }
  }
  rto = Rectangle{tp1x, tp1y, tp2x, tp2y};
  return (tp2x - tp1x + 1) * (tp2y - tp1y + 1);
}

void AssetHelper::Render(uint32_t* from, uint32_t* to, const Rectangle & rfrom, const Rectangle & rto)
{
  auto [fp1x, fp1y] = rfrom.p1();
  auto [fp2x, fp2y] = rfrom.p2();
  auto [tp1x, tp1y] = rto.p1();
  auto [tp2x, tp2y] = rto.p2();
  tp1x = std::clamp(tp1x, fp1x, fp2x);
  tp1y = std::clamp(tp1y, fp1y, fp2y);
  tp2x = std::clamp(tp2x, fp1x, fp2x);
  tp2y = std::clamp(tp2y, fp1y, fp2y);
  auto w = rto.width();
  for(size_t y = fp1y; y <= fp2y; ++y)
  {
    for(size_t x = fp1x; x <= fp2x; ++x)
    {
      to[x + y * w] = *from++;
    }
  }
}
