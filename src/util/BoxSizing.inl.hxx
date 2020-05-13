constexpr ScreenMeasurement operator""_lh(unsigned long long lh)
{
  return {0, 0, 0, 0, static_cast<int>(lh)};
}

constexpr ScreenMeasurement operator""_lw(unsigned long long lw)
{
  return {0, 0, 0, static_cast<int>(lw), 0};
}

constexpr ScreenMeasurement operator""_eh(unsigned long long eh)
{
  return { 0, 0, static_cast<int>(eh), 0, 0};
}

constexpr ScreenMeasurement operator""_ew(unsigned long long ew)
{
  return {0, static_cast<int>(ew), 0, 0, 0};
}

constexpr ScreenMeasurement operator""_px(unsigned long long px)
{
  return {static_cast<int>(px), 0, 0, 0, 0};
}

constexpr ScreenMeasurement operator+(const ScreenMeasurement & m1, const ScreenMeasurement & m2)
{
  return {
    m1.px + m2.px,
    m1.ew + m2.ew,
    m1.eh + m2.eh,
    m1.lw + m2.lw,
    m1.lh + m2.lh
  };
}

constexpr ScreenMeasurement operator-(const ScreenMeasurement & m1, const ScreenMeasurement & m2)
{
  return {
    m1.px - m2.px,
    m1.ew - m2.ew,
    m1.eh - m2.eh,
    m1.lw - m2.lw,
    m1.lh - m2.lh
  };
}

