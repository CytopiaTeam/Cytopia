#ifndef COLOR_HXX_
#define COLOR_HXX_

#include <cstdint>
#include <iostream>

class SDL_Color;

struct RGBAColor
{
public:
  constexpr RGBAColor(uint32_t color) : m_Color(color) {}
  int hue() const noexcept;
  int lightness() const noexcept;
  int saturation() const noexcept;
  uint8_t red() const noexcept;
  uint8_t green() const noexcept;
  uint8_t blue() const noexcept;
  uint8_t alpha() const noexcept;

  [[deprecated("Use sdl::toColor instead")]] SDL_Color to_SDL() const noexcept;

private:
  uint32_t m_Color;
};

constexpr RGBAColor operator""_rgba(unsigned long long val) { return RGBAColor{static_cast<uint32_t>(val)}; }

std::ostream &operator<<(std::ostream &, const RGBAColor &);

namespace Palette
{
constexpr RGBAColor Gray = 0x575757FF_rgba;
constexpr RGBAColor Orange = 0xFF771EFF_rgba;
constexpr RGBAColor Black = 0x000000FF_rgba;
constexpr RGBAColor White = 0xFFFFFFFF_rgba;
}; // namespace Palette

#endif // COLOR_HXX_
