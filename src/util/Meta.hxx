#ifndef META_HXX_
#define META_HXX_
#include <iostream>
#include <type_traits>
#include <variant>
#include <tuple>

using nullptr_t = std::nullptr_t;

template <bool value, typename ReturnType = void> using EnableIf = std::enable_if_t<value, ReturnType>;
template <typename Type> using Decay = std::decay_t<Type>;
template <typename Type, typename = void>
constexpr bool IsIterator = false;
template <typename Type>
constexpr bool IsIterator<Type, typename std::iterator_traits<Type>::iterator_category> = true;

/**
 * @struct TypeList
 * @brief Represent a list of types
 * @tparam Types the list of types
 */
template <typename... Types> struct TypeList;

template <typename T> struct TypeList<T>
{
  using current = T;
  using next = nullptr_t;
};

template <typename T, typename... Ts> struct TypeList<T, Ts...>
{
  using current = T;
  using next = TypeList<Ts...>;
};

template <size_t i, typename List, typename = void> struct GetType_t
{
  using type = nullptr_t;
};

template <size_t i, template <typename...> typename List, typename T, typename... Ts>
struct GetType_t<i, List<T, Ts...>, EnableIf<i == 0>>
{
  using type = T;
};

template <size_t i, template <typename...> typename List, typename T1, typename T2, typename... Ts>
struct GetType_t<i, List<T1, T2, Ts...>, EnableIf<i != 0>>
{
  using type = typename GetType_t<i - 1, List<T2, Ts...>>::type;
};

/**
 * @struct GetType
 * @brief GetType is the ith type in TypeList if
 *        within range, otherwise nullptr_t
 * @tparam i the index
 * @tparam List the TypeList
 */
template <auto i, typename List> using GetType = typename GetType_t<i, List>::type;

/**
 * @struct VariantType
 * @brief VariantType::type returns std::variant<All fields inside the TypeList>
 * @tparam List the TypeList
 */
template <class List> struct VariantType;

template <template <typename...> typename List, typename... Ts> struct VariantType<List<Ts...>>
{
  using type = std::variant<Ts...>;
};

/**
 * @struct TupleType
 * @brief TupleType::type returns std::tuple<All fields inside the TypeList>
 * @tparam List the TypeList
 */
template <class List> struct TupleType;

template <template <typename...> typename List, typename... Ts> struct TupleType<List<Ts...>>
{
  using type = std::tuple<Ts...>;
};

template <typename List, typename Type> constexpr bool ContainsType = false; // lgtm [cpp/use-in-own-initializer]

template <template <typename...> typename List, typename T1, typename T2, typename... Ts>
constexpr bool ContainsType<List<T1, Ts...>, T2> = std::is_same_v<Decay<T1>, Decay<T2>> ? true : ContainsType<List<Ts...>, T2>;

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
