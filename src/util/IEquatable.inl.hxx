
template <typename Type> IEquatable<Type>::IEquatable()
{
  using FunctorType = typename GetMemberType<decltype(&Type::Hash)>::type;
  static_assert(std::is_same_v<FunctorType, std::size_t() const noexcept>,
                "You must implement std::size_t Hash() const noexcept and make it accessible to IEquatable");
}

template <typename Type> constexpr bool IEquatable<Type>::operator!=(const IEquatable<Type> &other) const noexcept
{
  return m_Hash != other.m_Hash;
}

template <typename Type> bool IEquatable<Type>::operator==(const IEquatable<Type> &other) const noexcept
{
  if constexpr (is_strong_equatable_type<Type>::value)
  {
    return m_Hash == other.m_Hash && Type::Equals(other);
  }
  else
  {
    return m_Hash == other.m_Hash;
  }
}

template <typename Type> void IEquatable<Type>::onHashChanged(std::size_t hash) noexcept { m_Hash = hash; }
template <typename Type> void IEquatable<Type>::onHashChanged() noexcept { m_Hash = static_cast<Type &>(*this).Hash(); }
