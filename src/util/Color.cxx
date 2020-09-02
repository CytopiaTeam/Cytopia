#include "Color.hxx"
#include <algorithm>
#include <array>
#include <cmath>
#include <SDL.h>
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

float RGBAColor::hue() const noexcept
{
  std::array<uint8_t, 3> colors = {red(), green(), blue()};
  auto [min, max] = std::minmax_element(colors.data(), colors.data() + 3);
  uint16_t C = *max - *min;
  if (C == 0)
    return 0;
  float x = colors[(max - colors.data() + 1) % 3];
  x -= colors[(max - colors.data() + 2) % 3];
  x += 2.f * (max - colors.data()) * C;
  x /= 6.f;
  x /= C;
  return std::fmod(x + 1.f, 1.f);
}

float RGBAColor::lightness() const noexcept
{
  auto [min, max] = std::minmax({red(), green(), blue()});
  float l = min;
  l += max;
  l /= 510.f;
  return l;
}

float RGBAColor::saturation() const noexcept
{
  auto [min, max] = std::minmax({red(), green(), blue()});
  uint16_t L = max;
  L += min;
  if (L == 0 || L == 510)
    return 0;
  float C = max - min;
  C /= 255 - std::abs(L - 255);
  return C;
}

RGBAColor RGBAColor::fromHSLA(float h, float s, float l, uint8_t a)
{
  int k = static_cast<int>(h / 21) % 12;
  float m = s * std::min(l, 1.f - l);
  uint8_t r = 255 * (l - m * std::max(-1, std::min({k - 3, 9 - k, 1})));
  k += 4;
  k %= 12;
  uint8_t b = 255 * (l - m * std::max(-1, std::min({k - 3, 9 - k, 1})));
  k += 4;
  k %= 12;
  uint8_t g = 255 * (l - m * std::max(-1, std::min({k - 3, 9 - k, 1})));
  return {r, g, b, a};
}

std::ostream &operator<<(std::ostream &os, const RGBAColor &c)
{
  return os << "Color { red: " << static_cast<int>(c.red()) << ", green: " << static_cast<int>(c.green())
            << ", blue: " << static_cast<int>(c.blue()) << " }";
}
