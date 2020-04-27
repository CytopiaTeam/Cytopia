template <typename Factor>
constexpr ZoomFactor ZoomFactor::operator*(Factor f) const noexcept
{
  ZoomFactor zf = *this;
  zf.m_Zoom *= f;
  return zf;
}

template <typename Factor>
constexpr ZoomFactor ZoomFactor::operator/(Factor f) const noexcept
{
  ZoomFactor zf = *this;
  zf.m_Zoom /= f;
  return zf;
}

template <typename Factor>
constexpr ZoomFactor ZoomFactor::operator+(Factor f) const noexcept
{
  ZoomFactor zf = *this;
  zf.m_Zoom += f << (std::numeric_limits<IntegerType>::digits / 2);
  return zf;
}

template <typename Factor>
constexpr ZoomFactor ZoomFactor::operator-(Factor f) const noexcept
{
  ZoomFactor zf = *this;
  zf.m_Zoom -= f << (std::numeric_limits<IntegerType>::digits / 2);
  return zf;
}

template <typename Factor>
constexpr ZoomFactor & ZoomFactor::operator*=(Factor f) noexcept
{
  m_Zoom *= f;
  return *this;
}

template <typename Factor>
constexpr ZoomFactor & ZoomFactor::operator/=(Factor f) noexcept
{
  m_Zoom /= f;
  return *this;
}

template <typename Factor>
constexpr ZoomFactor & ZoomFactor::operator+=(Factor f) noexcept
{
  m_Zoom += f << (std::numeric_limits<IntegerType>::digits / 2);
  return *this;
}

template <typename Factor>
constexpr ZoomFactor & ZoomFactor::operator-=(Factor f) noexcept
{
  m_Zoom -= f << (std::numeric_limits<IntegerType>::digits / 2);
  return *this;
}

constexpr ZoomFactor::ZoomFactor() : ZoomFactor(0) { }

constexpr ZoomFactor::ZoomFactor(ZoomFactor::IntegerType i) :
  m_Zoom(i << (std::numeric_limits<IntegerType>::digits / 2))
{
}

constexpr ZoomFactor::operator IntegerType() const noexcept
{
  return m_Zoom >> (std::numeric_limits<IntegerType>::digits / 2);
}

constexpr bool ZoomFactor::operator==(ZoomFactor zf) const noexcept
{
  return m_Zoom == zf.m_Zoom;
}

constexpr bool ZoomFactor::operator!=(ZoomFactor zf) const noexcept
{
  return m_Zoom != zf.m_Zoom;
}

constexpr bool ZoomFactor::operator<(ZoomFactor zf) const noexcept
{
  return m_Zoom < zf.m_Zoom;
}

constexpr bool ZoomFactor::operator>(ZoomFactor zf) const noexcept
{
  return m_Zoom > zf.m_Zoom;
}

constexpr bool ZoomFactor::operator<=(ZoomFactor zf) const noexcept
{
  return m_Zoom <= zf.m_Zoom;
}

constexpr bool ZoomFactor::operator>=(ZoomFactor zf) const noexcept
{
  return m_Zoom >= zf.m_Zoom;
}
