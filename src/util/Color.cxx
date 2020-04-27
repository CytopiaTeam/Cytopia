#include "Color.hxx"
#include <algorithm>
#include <array>

SDL_Color RGBAColor::to_SDL() const noexcept
{
  return SDL_Color{
    static_cast<uint8_t>((m_Color >> 24) & 0xFF),
    static_cast<uint8_t>((m_Color >> 16) & 0xFF),
    static_cast<uint8_t>((m_Color >> 8) & 0xFF),
    static_cast<uint8_t>((m_Color >> 0) & 0xFF)
  };
}

uint8_t RGBAColor::hue() const noexcept
{
  std::array<uint8_t, 3> colors = {
    static_cast<uint8_t>((m_Color >> 24) & 0xFF),
    static_cast<uint8_t>((m_Color >> 16) & 0xFF),
    static_cast<uint8_t>((m_Color >> 8) & 0xFF)
  };
  auto it = std::max_element(
      colors.begin(), colors.end()) - colors.begin();
  if(colors[it] == 0) return 0;
  return 60 * (it * 2 + 
      (colors[(it + 1) % 3] - colors[(it + 2) % 3]) / colors[it]);
}

uint8_t RGBAColor::lightness() const noexcept
{
  auto [min, max] = std::minmax({
    static_cast<uint8_t>((m_Color >> 24) & 0xFF),
    static_cast<uint8_t>((m_Color >> 16) & 0xFF),
    static_cast<uint8_t>((m_Color >> 8) & 0xFF)
    });
  return (min + max) / 2;
}

uint8_t RGBAColor::saturation() const noexcept
{
  auto [min, max] = std::minmax({
    static_cast<uint8_t>((m_Color >> 24) & 0xFF),
    static_cast<uint8_t>((m_Color >> 16) & 0xFF),
    static_cast<uint8_t>((m_Color >> 8) & 0xFF)
    });
  auto C = max - min;
  if(max + min == 0) return 0;
  return C / (1 - std::abs(2 * max - C - 1));
}
