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

RGBAColor::operator uint32_t() const noexcept { return m_Color; }

uint8_t RGBAColor::green() const noexcept { return m_Color >> 16; }

uint8_t RGBAColor::blue() const noexcept { return m_Color >> 8; }

uint8_t RGBAColor::alpha() const noexcept { return m_Color; }

uint8_t RGBAColor::hue() const noexcept
{
  std::array<uint8_t, 3> colors = {red(), green(), blue()};
  auto [min, max] = std::minmax_element(colors.data(), colors.data() + 3);
  uint16_t C = *max - *min;
  if (C == 0)
    return 0;
  uint16_t x = colors[(max - colors.data() + 1) % 3];
  x -= colors[(max - colors.data() + 2) % 3];
  x += 2 * (max - colors.data()) * C;
  x *= 42;
  x /= C;
  x %= 256;
  return static_cast<uint8_t>(x);
}

uint8_t RGBAColor::lightness() const noexcept
{
  auto [min, max] = std::minmax({red(), green(), blue()});
  uint16_t l = min;
  l += max;
  l /= 2;
  return static_cast<uint8_t>(l);
}

uint8_t RGBAColor::saturation() const noexcept
{
  auto [min, max] = std::minmax({red(), green(), blue()});
  uint16_t L = max;
  L += min;
  if (L == 0 || L == 510)
    return 0;
  uint16_t C = max - min;
  C *= 255;
  C /= 255 - std::abs(L - 255);
  return static_cast<uint8_t>(C);
}

RGBAColor RGBAColor::fromHSLA(uint8_t h, uint8_t s, uint8_t l, uint8_t a)
{
  uint8_t k = (h / 21) % 12;
  uint16_t m = s * std::min(l, static_cast<uint8_t>(255 - l));
  uint8_t r = l - m * std::max(-1, std::min({k - 3, 9 - k, 1})) / 255;
  k += 4;
  k %= 12;
  uint8_t b = l - m * std::max(-1, std::min({k - 3, 9 - k, 1})) / 255;
  k += 4;
  k %= 12;
  uint8_t g = l - m * std::max(-1, std::min({k - 3, 9 - k, 1})) / 255;
  return {r, g, b, a};
}

std::ostream &operator<<(std::ostream &os, const RGBAColor &c)
{
  return os << "Color { red: " << static_cast<int>(c.red()) << ", green: " << static_cast<int>(c.green())
            << ", blue: " << static_cast<int>(c.blue()) << " }";
}
