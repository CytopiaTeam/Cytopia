#ifndef IEQUATABLE_HXX_
#define IEQUATABLE_HXX_

#include "Meta.hxx"

template <typename Type, typename = void> struct IsStrongEquatableType : public std::false_type
{
};

template <typename Type>
struct IsStrongEquatableType<Type, std::void_t<decltype(std::declval<Type>().Equals(std::declval<Type>()))>> : std::true_type
{
};

/**
 * @brief Allows Type to be hashable, use operator==, and use operator!=
 * @details First, Type should implement std::size_t Hash() const noexcept.
 *          Whenever your data is mutated, including at constructor-time, you must call
 *          onHashChanged(). If you are concerned with hash conflicts,
 *          then Type should implement bool Equals(const Type& other) const noexcept and
 *          operator== will also verify for strong equality. Otherwise, it will only
 *          compare hashes. This abstraction is especially efficient for compound types.
 * @tparam Type the data type that must be Equatable
 */
template <typename Type> class IEquatable
{

private:
  std::size_t m_Hash;

public:
  IEquatable()
  {
    using FunctorType = typename GetMemberType<decltype(&Type::Hash)>::type;
    static_assert(std::is_same_v<FunctorType, std::size_t() const noexcept>,
                  "You must implement std::size_t Hash() const noexcept and make it accessible to IEquatable");
  }

  inline constexpr bool operator!=(const IEquatable<Type> &other) const noexcept { return m_Hash != other.m_Hash; }

  inline bool operator==(const IEquatable<Type> &other) const noexcept
  {
    if constexpr (IsStrongEquatableType<Type>::value)
    {
      return m_Hash == other.m_Hash && Type::Equals(other);
    }
    else
    {
      return m_Hash == other.m_Hash;
    }
  }

protected:
  inline void onHashChanged(std::size_t hash) noexcept { m_Hash = hash; }
  inline void onHashChanged() noexcept { m_Hash = static_cast<Type &>(*this).Hash(); }
};

#endif
