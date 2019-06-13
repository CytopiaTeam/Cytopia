#ifndef META_HXX_
#define META_HXX_
#include <iostream>
#include <type_traits>
#include <variant>

/**
 * @struct TypeList
 * @brief Represent a list of types
 * @tparam Types the list of types
 */
template  <typename... Types>
struct TypeList;

template  <typename T>
struct TypeList<T>
{
  using current = T;
  using next = nullptr_t;
};

template  <typename T, typename... Ts>
struct TypeList<T, Ts...>
{
  using current = T;
  using next = TypeList<Ts...>;
};

/**
 * @struct GetType
 * @brief GetType::type returns the ith type in TypeList if
 *        within range, otherwise nullptr_t
 * @tparam i the index
 * @tparam List the TypeList
 * @tparam unnamed don't use it
 */
template <size_t i, typename List, typename = void>
struct GetType;

template <size_t, typename, typename = void>
struct GetType
{
  using type = nullptr_t;
};

template <size_t i, template<typename...> typename List, typename T, typename... Ts>
struct GetType <i, List<T, Ts...>,
  typename std::enable_if_t<i == 0> >
{
  using type = T;
};

template <size_t i, template<typename...> typename List, typename T1, typename T2, typename... Ts>
struct GetType<i, List<T1, T2, Ts...>,
  typename std::enable_if_t<i != 0> >
{
  using type = typename GetType<i - 1, List<T2, Ts...> >::type;
};

/**
 * @struct VariantType
 * @brief VariantType::type returns std::variant<All fields inside the TypeList>
 * @tparam List the TypeList
 */
template <class List>
struct VariantType;

template <template<typename...> typename List, typename... Ts>
struct VariantType<List<Ts...>>
{
  using type = std::variant<Ts...>;
};

/**
 * @struct Constant
 * @brief Constant::value returns the value
 * @tparam Type the type of the constant
 * @tparam value the constant value
 */
template<typename Type, T value>
using Constant = std::integral_constant<Type, value>;

#endif
