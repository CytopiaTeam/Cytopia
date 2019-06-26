#ifndef META_HXX_
#define META_HXX_
#include <iostream>
#include <type_traits>
#include <variant>

using nullptr_t = std::nullptr_t;

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
struct GetType_t<i, List<T, Ts...>, typename std::enable_if_t<i == 0>>
{
  using type = T;
};

template <size_t i, template <typename...> typename List, typename T1, typename T2, typename... Ts>
struct GetType_t<i, List<T1, T2, Ts...>, typename std::enable_if_t<i != 0>>
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

template <typename List, typename Type> constexpr bool ContainsType = false;

template <template <typename...> typename List, typename T1, typename T2, typename... Ts>
constexpr bool ContainsType<List<T1, Ts...>, T2> = std::is_same_v<T1, T2> ? true : ContainsType<List<Ts...>, T2>;

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

#endif
