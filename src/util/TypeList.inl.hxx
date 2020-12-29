
namespace _typelist_impl
{

template <typename Target, typename... Types> struct _ContainsType
{
  static constexpr bool value = false;
};

template <typename Target, typename T1, typename... Ts> struct _ContainsType<Target, T1, Ts...>
{
  static constexpr bool value = std::is_same_v<Target, T1> ? true : _ContainsType<Target, Ts...>::value;
};

template <typename Value, template <typename, typename> typename MetaFunction, typename... Types> struct _Reduce
{
  using type = Value;
};

template <typename Value, template <typename, typename> typename MetaFunction, typename T1, typename... Ts>
struct _Reduce<Value, MetaFunction, T1, Ts...>
{
  using type = typename _Reduce<typename MetaFunction<Value, T1>::type, MetaFunction, Ts...>::type;
};

template <int N, typename... Types> struct _Get
{
  static_assert(sizeof...(Types) < 0, "TypeList::Get is out of bounds");
};

template <int N, typename T1, typename... Ts> struct _Get<N, T1, Ts...>
{
  using type = typename _Get<N - 1, Ts...>::type;
};

template <typename T1, typename... Ts> struct _Get<0, T1, Ts...>
{
  using type = T1;
};

template <typename = void, typename... Types> struct _CommonType
{
  using type = std::variant<Types...>;
};

template <typename... Types> struct _CommonType<std::void_t<typename std::common_type<Types...>::type>, Types...>
{
  using type = typename std::common_type<Types...>::type;
};

}; // namespace _typelist_impl
