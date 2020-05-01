#include "Color.hxx"
#include <algorithm>
#include <array>
#include <SDL2/SDL.h>
#include "LOG.hxx"

SDL_Color RGBAColor::to_SDL() const noexcept
{
  return SDL_Color {
    static_cast<uint8_t>((m_Color >> 24) & 0xFF),
    static_cast<uint8_t>((m_Color >> 16) & 0xFF),
    static_cast<uint8_t>((m_Color >>  8) & 0xFF),
    static_cast<uint8_t>((m_Color >>  0) & 0xFF)
  };
}

uint8_t RGBAColor::red() const noexcept
{
  return m_Color >> 24;
}

uint8_t RGBAColor::green() const noexcept
{
  return (m_Color >> 16) & 0xFF;
}

uint8_t RGBAColor::blue() const noexcept
{
  return (m_Color >> 8) & 0xFF;
}

uint8_t RGBAColor::alpha() const noexcept
{
  return m_Color & 0xFF;
}

int RGBAColor::hue() const noexcept
{
  std::array<uint8_t, 3> colors = { red(), green(), blue() };
  auto it = std::max_element(
      colors.begin(), colors.end()) - colors.begin();
  if(colors[it] == 0) return 0;
  int x = colors[(it + 1) % 3];
  x -= colors[(it + 2) % 3];
  x /= colors[it];
  x += 2 * it;
  x *= 60;
  x += 360;
  x %= 360;
  return std::move(x);
}

int RGBAColor::lightness() const noexcept
{
  auto [min, max] = std::minmax({ red(), green(), blue() });
  return (static_cast<int>(min) + static_cast<int>(max)) * 100 / 510;
}

int RGBAColor::saturation() const noexcept
{
  auto [min, max] = std::minmax({ red(), green(), blue() });
  if(max + min == 0 || max + min == 510) return 0;
  int C = max - min;
  C *= 100;
  C /= 255 - std::abs(2 * max - C / 100 - 255);
  return std::move(C);
}

std::ostream & operator<<(std::ostream & os, const RGBAColor & c)
{
  return os << "Color { red: " 
    << static_cast<int>(c.red()) << ", green: " 
    << static_cast<int>(c.green()) << ", blue: " 
    << static_cast<int>(c.blue()) << " }";
}
