#ifndef COLOR_HXX_
#define COLOR_HXX_

#include <cstdint>
#include <SDL2/SDL.h>

struct RGBAColor
{
  uint32_t m_Color;

public:
  constexpr RGBAColor(uint32_t color) : m_Color(color) {}
  [[deprecated("Use sdl::convert")]] SDL_Color to_SDL() const noexcept;
  uint8_t hue() const noexcept;
  uint8_t lightness() const noexcept;
  uint8_t saturation() const noexcept;
};

constexpr RGBAColor operator""_rgba(unsigned long long val) { return RGBAColor{static_cast<uint32_t>(val)}; }

namespace Palette
{
constexpr RGBAColor Gray = 0x575757FF_rgba;
constexpr RGBAColor Orange = 0xff9e0dFF_rgba;
constexpr RGBAColor Black = 0x000000FF_rgba;
constexpr RGBAColor White = 0xFFFFFFFF_rgba;
}; // namespace Palette

#endif // COLOR_HXX_
