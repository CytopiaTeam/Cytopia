#include "Color.hxx"
#include <algorithm>
#include <array>
#include <SDL2/SDL.h>
#include "LOG.hxx"

SDL_Color RGBAColor::to_SDL() const noexcept
{
  return SDL_Color{static_cast<uint8_t>(m_Color >> 24), static_cast<uint8_t>(m_Color >> 16), static_cast<uint8_t>(m_Color >> 8),
                   static_cast<uint8_t>(m_Color)};
}

uint8_t RGBAColor::red() const noexcept { return m_Color >> 24; }

uint8_t RGBAColor::green() const noexcept { return m_Color >> 16; }

uint8_t RGBAColor::blue() const noexcept { return m_Color >> 8; }

uint8_t RGBAColor::alpha() const noexcept { return m_Color; }

int RGBAColor::hue() const noexcept
{
  std::array<uint8_t, 3> colors = {red(), green(), blue()};
  auto [min, max] = std::minmax_element(colors.data(), colors.data() + 3);
  int C = *max - *min;
  if (C == 0)
    return 0;
  int x = colors[(max - colors.data() + 1) % 3];
  x -= colors[(max - colors.data() + 2) % 3];
  x += 2 * (max - colors.data()) * C;
  x *= 60;
  x /= C;
  x += 360;
  x %= 360;
  return x;
}

int RGBAColor::lightness() const noexcept
{
  auto [min, max] = std::minmax({red(), green(), blue()});
  return (static_cast<int>(min) + static_cast<int>(max)) * 100 / 510;
}

int RGBAColor::saturation() const noexcept
{
  auto [min, max] = std::minmax({red(), green(), blue()});
  int L = max + min;
  if (L == 0 || L == 510)
    return 0;
  int C = max - min;
  C *= 100;
  C /= 255 - std::abs(L - 255);
  return C;
}

std::ostream &operator<<(std::ostream &os, const RGBAColor &c)
{
  return os << "Color { red: " << static_cast<int>(c.red()) << ", green: " << static_cast<int>(c.green())
            << ", blue: " << static_cast<int>(c.blue()) << " }";
}
