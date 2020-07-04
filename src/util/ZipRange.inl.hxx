/* Deduction guides for range initialization */
template <typename Range, typename... Ranges>
ZipRange(Range &&, Ranges &&...)
    -> ZipRange<decltype(std::declval<Range>().begin()), decltype(std::declval<Ranges>().begin())...>;

template <typename Range, typename... Ranges>
ZipRange(const Range &&, const Ranges &&...)
    -> ZipRange<decltype(std::declval<Range>().begin()), decltype(std::declval<Ranges>().begin())...>;

template <typename Iterator, typename... Iterators>
template <typename Range, typename... Ranges>
ZipRange<Iterator, Iterators...>::ZipRange(Range &&r1, Ranges &&... ranges)
    : m_Begin(r1.begin(), ranges.begin()...), m_End(r1.end(), ranges.end()...)
{
}

template <typename Iterator, typename... Iterators>
template <typename Range, typename... Ranges>
ZipRange<Iterator, Iterators...>::ZipRange(const Range &&r1, const Ranges &&... ranges)
{
  static_assert(sizeof(Range) < 0, "You cannot construct a ZipRange from temporary containers");
}

template <typename Iterator, typename... Iterators>
typename ZipRange<Iterator, Iterators...>::ZipIterator ZipRange<Iterator, Iterators...>::begin() const
{
  return m_Begin;
}

template <typename Iterator, typename... Iterators>
typename ZipRange<Iterator, Iterators...>::ZipIterator ZipRange<Iterator, Iterators...>::end() const
{
  return m_End;
}

template <typename Iterator, typename... Iterators>
ZipRange<Iterator, Iterators...>::ZipIterator::ZipIterator(Iterator it, Iterators... its) : m_It(it, its...)
{
}

template <typename Iterator, typename... Iterators>
bool ZipRange<Iterator, Iterators...>::ZipIterator::operator==(const ZipIterator &other) const
{
  return std::apply(
      [](Iterator it, Iterators... its, Iterator it2, Iterators... its2) { return it == it2 || ((its == its2) || ...); },
      std::tuple_cat(m_It, other.m_It));
}

template <typename Iterator, typename... Iterators>
bool ZipRange<Iterator, Iterators...>::ZipIterator::operator!=(const ZipIterator &other) const
{
  return std::apply(
      [](Iterator it, Iterators... its, Iterator it2, Iterators... its2) { return it != it2 && ((its != its2) && ...); },
      std::tuple_cat(m_It, other.m_It));
}

template <typename Iterator, typename... Iterators>
typename ZipRange<Iterator, Iterators...>::ZipIterator &ZipRange<Iterator, Iterators...>::ZipIterator::operator++()
{
  std::apply(
      [](Iterator &it, Iterators &... its) {
        ++it;
        (++its, ...);
      },
      m_It);
  return *this;
}

template <typename Iterator, typename... Iterators>
typename ZipRange<Iterator, Iterators...>::ZipIterator ZipRange<Iterator, Iterators...>::ZipIterator::operator++(int)
{
  ZipIterator other{*this};
  std::apply(
      [](Iterator &it, Iterators &... its) {
        ++it;
        (++its, ...);
      },
      m_It);
  return other;
}

template <typename Iterator, typename... Iterators>
typename ZipRange<Iterator, Iterators...>::ZipIterator::reference ZipRange<Iterator, Iterators...>::ZipIterator::operator*()
{
  return std::apply([](Iterator it, Iterators... its) { return reference{*it, *its...}; }, m_It);
}
