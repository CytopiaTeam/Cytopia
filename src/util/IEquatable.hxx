#ifndef IEQUATABLE_HXX_
#define IEQUATABLE_HXX_

#include "Meta.hxx"

/**
 * @brief Is true if a type is strongly equatable
 */
template <typename Type, typename = void> struct is_strong_equatable_type : public std::false_type
{
};

template <typename Type>
struct is_strong_equatable_type<Type, std::void_t<decltype(std::declval<Type>().Equals(std::declval<Type>()))>> : std::true_type
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
  /**
   * @brief Create an Equatable type
   */
  IEquatable();

  /**
   * @returns true if not equal
   */
  constexpr bool operator!=(const IEquatable<Type> &other) const noexcept;

  /**
   * @returns true if equal
   */
  bool operator==(const IEquatable<Type> &other) const noexcept;

protected:
  /**
   * @brief Call when the hash has changed
   * @param hash the new hash
   */
  void onHashChanged(std::size_t hash) noexcept;

  /**
   * @brief Call when the hash has changed
   */
  void onHashChanged() noexcept;
};

#include "IEquatable.inl.hxx"

#endif
