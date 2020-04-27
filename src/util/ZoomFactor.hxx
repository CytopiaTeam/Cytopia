#ifndef ZOOM_FACTOR_HXX
#define ZOOM_FACTOR_HXX

#include <cstdint>
#include <limits>
#include <cmath>

class ZoomFactor
{
public:
  /* Change this for better precision */
  using IntegerType = uint32_t;

  constexpr ZoomFactor();
  constexpr ZoomFactor(IntegerType);

  constexpr operator IntegerType() const noexcept;

  /***********************
   *      Arithmetic     *
   ***********************/
  
  template <typename Factor>
  constexpr ZoomFactor operator*(Factor) const noexcept;
  template <typename Factor>
  constexpr ZoomFactor operator/(Factor) const noexcept;
  template <typename Factor>
  constexpr ZoomFactor operator+(Factor) const noexcept;
  template <typename Factor>
  constexpr ZoomFactor operator-(Factor) const noexcept;
  
  template <typename Factor>
  constexpr ZoomFactor &operator*=(Factor) noexcept;
  template <typename Factor>
  constexpr ZoomFactor &operator/=(Factor) noexcept;
  template <typename Factor>
  constexpr ZoomFactor &operator+=(Factor) noexcept;
  template <typename Factor>
  constexpr ZoomFactor &operator-=(Factor) noexcept;

  /***********************
   *      Comparison     *
   ***********************/
  
  constexpr bool operator==(ZoomFactor) const noexcept;
  constexpr bool operator!=(ZoomFactor) const noexcept;
  constexpr bool operator<(ZoomFactor) const noexcept;
  constexpr bool operator>(ZoomFactor) const noexcept;
  constexpr bool operator<=(ZoomFactor) const noexcept;
  constexpr bool operator>=(ZoomFactor) const noexcept;
private:
  IntegerType m_Zoom;
};

template <>
class std::numeric_limits<ZoomFactor>
{
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signalling_NaN = false;
  static constexpr bool has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr bool round_style = std::round_to_nearest;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = std::numeric_limits<ZoomFactor::IntegerType>::is_modulo;
  static constexpr int digits = std::numeric_limits<ZoomFactor::IntegerType>::digits;
  static constexpr int digits10 = std::numeric_limits<ZoomFactor::IntegerType>::digits10;
  static constexpr int max_digits10 = std::numeric_limits<ZoomFactor::IntegerType>::max_digits10;
  static constexpr int radix = std::numeric_limits<ZoomFactor::IntegerType>::radix;
  static constexpr int min_exponent = -digits/2 + 1;
  static constexpr int min_exponent10 = -digits10/2;
  static constexpr int max_exponent = digits/2 + 1;
  static constexpr int max_exponent10 = digits10/2;
  static constexpr bool traps = std::numeric_limits<ZoomFactor::IntegerType>::traps;
  static constexpr bool tinyness_before = false;
  
  static constexpr ZoomFactor min();
  static constexpr ZoomFactor lowest();
  static constexpr ZoomFactor max();
  static constexpr ZoomFactor epsilon();
  static constexpr ZoomFactor round_error();
  static constexpr ZoomFactor infinity();
  static constexpr ZoomFactor quiet_NaN();
  static constexpr ZoomFactor signaling_NaN();
  static constexpr ZoomFactor denorm_min();
};

#include "ZoomFactor.inl.hxx"

#endif // ZOOM_FACTOR_HXX
