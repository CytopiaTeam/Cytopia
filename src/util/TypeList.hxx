#ifndef TYPE_LIST_HXX_
#define TYPE_LIST_HXX_

#include <variant>
#include <tuple>
#include <type_traits>
#include "Exception.hxx"
#include "LOG.hxx"
#include "TypeList.inl.hxx"

template <typename Type> struct IntegralTypeSwitcher;

template <typename BaseType, template <typename> typename Comparator = IntegralTypeSwitcher, typename... Types> struct TypeSwitch;

/**
 * @struct TypeList
 * @brief Represent a list of types
 * @tparam Types the types of the list
 */
template <typename... Types> struct TypeList
{

  /**
   *  @returns true if Target is contained in the TypeList
   */
  template <typename Target> static constexpr bool ContainsType = _typelist_impl::_ContainsType<Target, Types...>::value;

  /**
   * @brief Applies MetaFunction over all types
   * @returns a TypeList containing the result of MetaFunction
   */
  template <template <typename> typename MetaFunction> using Map = TypeList<typename MetaFunction<Types>::type...>;

  /**
   * @brief Applies MetaFunction over all types
   * @returns a TypeList containing the result of MetaFunction applied on each Type
   */
  template <template <typename, typename> typename MetaFunction, typename Default = std::nullptr_t>
  using Reduce = typename _typelist_impl::_Reduce<Default, MetaFunction, Types...>::type;

  /**
   * @returns the Nth type or will fail
   *          if no such type exists
   */
  template <int N> using Get = typename _typelist_impl::_Get<N, Types...>::type;

  /**
   * @brief   Calls MetaFunctor<Type>::run(args...) for each 
   *          Type in the TypeList
   */
  template <template <typename> typename MetaFunctor, typename... Args> static constexpr void ForEach(Args &&... args)
  {
    (MetaFunctor<Types>::run(std::forward<decltype(args)>(args)...), ...);
  }

  /**
   * @returns   The variant of all types in the TypeList
   */
  using VariantType = std::variant<Types...>;

  /**
   * @returns   The tuple of all types in the TypeList
   */
  using TupleType = std::tuple<Types...>;

  /**
   * @returns   A common type to represent all types
   * @details   Returns a type that all types can be converted to.
   *            If none exists, returns ::VariantType
   */
  using CommonType = typename _typelist_impl::_CommonType<void, Types...>::type;

  /**
   * @brief   Create a TypeSwitch between all Types
   * @tparam  BaseType a common base type that all Types can be converted to
   * @tparam  Comparator the metafunctor which decides which type to return
   */
  template <typename BaseType, template <typename> typename Comparator = IntegralTypeSwitcher>
  using TypeSwitch = TypeSwitch<BaseType, Comparator, Types...>;
};

template <typename Type> struct is_typelist : public std::false_type
{
};

template <typename... Types> struct is_typelist<TypeList<Types...>> : public std::true_type
{
};

template <typename Type> struct is_variant : public std::false_type
{
};

template <typename... Types> struct is_variant<std::variant<Types...>> : public std::true_type
{
};

template <typename Type> struct is_tuple : public std::false_type
{
};

template <typename... Types> struct is_tuple<std::tuple<Types...>> : public std::true_type
{
};

template <typename T1> struct IntegralTypeSwitcher
{
  bool operator()(std::size_t &n) { return n-- == 0; }
  bool operator()(int &n) { return n-- == 0; }
};

template <typename BaseType, template <typename> typename Comparator, typename... Types> struct TypeSwitch
{
  template <typename... Args> BaseType operator()(Args...) { throw RuntimeError{TRACE_INFO "Index is out of bounds"}; }
};

template <typename BaseType, template <typename> typename Comparator, typename T1, typename... Ts>
struct TypeSwitch<BaseType, Comparator, T1, Ts...>
{
  template <typename... Args> BaseType operator()(Args... args)
  { // Must take by value
    return call(args...);
  }

private:
  template <typename... Args> BaseType call(Args &... args)
  {
    Comparator<T1> cmp;
    if (cmp(std::forward<decltype(args)>(args)...))
    {
      return BaseType(new T1);
    }
    else
    {
      TypeSwitch<BaseType, Comparator, Ts...> ts;
      return ts(std::forward<decltype(args)>(args)...);
    }
  }
};

#endif // TYPE_LIST_HXX_
