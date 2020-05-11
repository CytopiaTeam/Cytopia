
template <typename OutputIterator>
std::pair<int, int> AssetHelper::LoadImagePixels(const std::string & filepath, OutputIterator && it)
{
  auto surface = IMG_Load(filepath.c_str());
  if(!surface)
  {
    LOG(LOG_WARNING) << TRACE_INFO "Couldn't load image pixels: " << IMG_GetError();
    return {0, 0};
  }
  auto w = surface->w, h = surface->h;
  SDL_LockSurface(surface);
  auto format = surface->format;
  uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
  for(std::size_t i = 0; i < w * h; ++i)
  {
    *it++ = 
      (pixels[i] & format->Rmask) >> format->Rshift << 24 |
      (pixels[i] & format->Gmask) >> format->Gshift << 16 |
      (pixels[i] & format->Bmask) >> format->Bshift << 8  |
      (pixels[i] & format->Amask) >> format->Ashift << 0;
  }
  SDL_UnlockSurface(surface);
  return {w, h};
}

template <typename Iterator>
void AssetHelper::ColorMagicPixels(Iterator begin, Iterator end, const RGBAColor & color)
{
  uint8_t hue = color.hue();
  hue /= 60;  // Now hue is in [0, 5]
  
  while(begin != end)
  {
    if(((*begin >> 8) & 0xFF) == ((*begin >> 24) & 0xFF))
    {
      /* This is a magic pixel */
      auto [min, max] = std::minmax({
          (*begin >> 8) & 0xFF, 
          (*begin >> 16) & 0xFF, 
          (*begin >> 24) & 0xFF}
      );
      uint8_t alpha = *begin & 0xFF;
      uint8_t chroma = max - min;
      uint8_t X = chroma * (1 - std::abs(hue % 2 - 1));
      uint8_t lightness = (max + min) / 2;
      uint8_t m = lightness - chroma / 2;
      switch(hue)
      {
        case 0:
          *begin = m * 0x01010100 
            + (chroma << 24)
            + (X << 16)
            + alpha;
          break;
        case 1:
          *begin = m * 0x01010100 
            + (chroma << 16)
            + (X << 24)
            + alpha;
          break;
        case 2:
          *begin = m * 0x01010100 
            + (chroma << 16)
            + (X << 8)
            + alpha;
          break;
        case 3:
          *begin = m * 0x01010100 
            + (chroma << 8)
            + (X << 16)
            + alpha;
          break;
        case 4:
          *begin = m * 0x01010100 
            + (chroma << 8)
            + (X << 16)
            + alpha;
          break;
        case 5:
          *begin = m * 0x01010100 
            + (chroma << 16)
            + (X << 8)
            + alpha;
          break;
      }
    }
    ++begin;
  }
}
