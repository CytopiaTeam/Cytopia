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
  
constexpr ZoomFactor std::numeric_limits<ZoomFactor>::min()
{
  return ZoomFactor{1} / (ZoomFactor::IntegerType(1) << (digits / 2));
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::lowest()
{
  return {0};
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::max()
{
  return ZoomFactor{1} * ZoomFactor::IntegerType(-1);
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::epsilon()
{
  return min();
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::round_error()
{
  return ZoomFactor{1} / ZoomFactor::IntegerType(2);
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::infinity()
{
  return ZoomFactor{0};
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::quiet_NaN()
{
  return ZoomFactor{0};
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::signaling_NaN()
{
  return ZoomFactor{0};
}

constexpr ZoomFactor std::numeric_limits<ZoomFactor>::denorm_min()
{
  return ZoomFactor{0};
}
