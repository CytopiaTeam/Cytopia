#ifndef META_HXX_
#define META_HXX_
#include <iostream>
#include <type_traits>

using nullptr_t = std::nullptr_t;

template <bool value, typename ReturnType = void> using EnableIf = std::enable_if_t<value, ReturnType>;

/**
 * @struct Constant
 * @brief Constant::value returns the value
 * @tparam val the constant value
 * @pre Type of val must be an integral type
 */
template <auto val, typename Type = decltype(val)> using Constant = std::integral_constant<Type, val>;

template <typename> struct GetMemberType
{
};

template <typename Type, typename Member> struct GetMemberType<Member Type::*>
{
  using type = Member;
};

template <typename WeakType, typename> struct StrongType
{
  StrongType() = default;
  StrongType(const StrongType &) = default;
  StrongType(StrongType &&) = default;
  StrongType &operator=(const StrongType &) = default;
  StrongType &operator=(StrongType &&) = default;
  StrongType &operator=(const WeakType &weak)
  {
    m_Data = weak;
    return *this;
  }
  StrongType &operator=(WeakType &&weak)
  {
    m_Data = weak;
    return *this;
  }

  template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<WeakType, Args...>>>
  explicit inline StrongType(Args &&... args) : m_Data(std::forward<Args>(args)...)
  {
  }

  WeakType &get() noexcept { return m_Data; }
  const WeakType &get() const noexcept { return m_Data; }

  friend bool operator==(const StrongType &s1, const StrongType &s2) noexcept { return s1.m_Data == s2.m_Data; }

  friend std::ostream &operator<<(std::ostream &os, const StrongType &obj) { return os << obj.m_Data; }

  friend std::istream &operator>>(std::istream &is, StrongType &obj) { return is >> obj.m_Data; }

private:
  WeakType m_Data;
};

template <typename WeakType, typename Tag> struct std::hash<StrongType<WeakType, Tag>>
{
  std::size_t operator()(const StrongType<WeakType, Tag> &key) const { return std::hash<WeakType>{}(key.get()); }
};

#endif
