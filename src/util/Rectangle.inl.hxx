#include <algorithm>
#include "LOG.hxx"

constexpr Rectangle::Rectangle(int x1, int y1, int x2, int y2)
    : m_x1(std::min(x1, x2)), m_y1(std::min(y1, y2)), m_x2(std::max(x1, x2)), m_y2(std::max(y1, y2))
{
}

template <typename Factor> void Rectangle::scaleCenter(Factor factor) noexcept
{
  if (factor == Factor(0) || m_x1 == -1)
  {
    *this = EMPTY();
    return;
  }
  int avgx = (m_x1 + m_x2) / 2;
  int avgy = (m_y1 + m_y2) / 2;
  factor /= 2;

  m_x1 = avgx - (avgx - m_x1 + 1) * factor + 1;
  m_x2 = (m_x2 - avgx + 1) * factor + avgx;
  m_y1 = avgy - (avgy - m_y1 + 1) * factor + 1;
  m_y2 = (m_y2 - avgy + 1) * factor + avgy;

  if (m_x2 < m_x1 || m_y2 < m_y1)
  {
    *this = EMPTY();
  }
}
