#include <algorithm>

constexpr Rectangle::Rectangle(int x1, int y1, int x2, int y2) :
  m_x1(std::min(x1, x2)),
  m_y1(std::min(y1, y2)),
  m_x2(std::max(x1, x2)),
  m_y2(std::max(y1, y2))
{ }

