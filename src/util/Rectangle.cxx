#include <iostream>
#include "Rectangle.hxx"
#include "../util/LOG.hxx"
#include "../util/Point.hxx"

bool Rectangle::contains(const Point2D & p) const noexcept
{
  return p.x <= m_x2 && p.x >= m_x1 && p.y <= m_y2 && p.y >= m_y1;
}

Rectangle Rectangle::getBounds() const noexcept
{
  return *this;
}

SDL_Rect Rectangle::to_SDL() const noexcept
{
  return SDL_Rect{m_x1, m_y1, m_x2 - m_x1, m_y2 - m_y1};
}

int Rectangle::width() const noexcept
{
  return m_x2 - m_x1 + 1;
}

int Rectangle::height() const noexcept
{
  return m_y2 - m_y1 + 1;
}

std::pair<int, int> Rectangle::p1() const noexcept { return {m_x1, m_y1}; }

std::pair<int, int> Rectangle::p2() const noexcept { return {m_x2, m_y2}; }

int Rectangle::x1() const noexcept { return m_x1; }

int Rectangle::y1() const noexcept { return m_y1; }

int Rectangle::x2() const noexcept { return m_x2; }

int Rectangle::y2() const noexcept { return m_y2; }

bool Rectangle::isEmpty() const noexcept { return m_x1 == -1; }

void Rectangle::translateX(int x) noexcept
{
  m_x1 += x;
  m_x2 += x;
}

void Rectangle::translateY(int y) noexcept
{
  m_y1 += y;
  m_y2 += y;
}

void Rectangle::intersect(const Rectangle & other) noexcept
{
  auto [op1x, op1y] = other.p1();
  auto [op2x, op2y] = other.p2();
  if(m_x2 < op1x || op2x < m_x1 || m_y2 < op1y || op2y < m_y1 || op1x == -1)
  {
    // Empty intersection
    m_x1 = -1;
    return;
  }
  m_x1 = std::clamp(m_x1, op1x, op2x);
  m_y1 = std::clamp(m_y1, op1y, op2y);
  m_x2 = std::clamp(m_x2, op1x, op2x);
  m_y2 = std::clamp(m_y2, op1y, op2y);
}

void Rectangle::scaleCenter(ZoomFactor zoom) noexcept
{
  if(zoom == ZoomFactor{0})
  {
    m_x1 = -1;
    return;
  }
  int avgx = (m_x1 + m_x2) / 2;
  int avgy = (m_y1 + m_y2) / 2;
  m_x1 = avgx - (((avgx - m_x1) * zoom) >> 16);
  m_x2 = (((m_x2 - avgx) * zoom) >> 16) + avgx;
  m_y1 = avgy - (((avgy - m_y1) * zoom) >> 16);
  m_y2 = (((m_y2 - avgy) * zoom) >> 16) + avgy;
}

std::ostream& operator<<(std::ostream& os, const Rectangle& r)
{
  return os << "Rectangle { x1: " << r.m_x1 
    << ", x2: " << r.m_x2 << ", y1: " << r.m_y1
    << ", y2: " << r.m_y2 << " }";
}
