#ifndef ZIP_RANGE_HXX_
#define ZIP_RANGE_HXX_

#include <iterator>

#include "Meta.hxx"

template <typename Iterator, typename... Iterators>
class ZipRange
{
public:

  class ZipIterator
  {
    using TupleIterator = std::tuple<Iterator, Iterators...>;
    TupleIterator m_It;
  public:
    using value_type = std::tuple<
      typename std::iterator_traits<Iterator>::value_type, 
      typename std::iterator_traits<Iterators>::value_type ...>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::tuple<
      typename std::iterator_traits<Iterator>::pointer, 
      typename std::iterator_traits<Iterators>::pointer ...>;
    using reference = std::tuple<
      typename std::iterator_traits<Iterator>::reference, 
      typename std::iterator_traits<Iterators>::reference ...>;
    using iterator_category = std::forward_iterator_tag;
    ZipIterator(Iterator, Iterators ...);
    bool operator==(ZipIterator other);
    bool operator!=(ZipIterator other);
    ZipIterator & operator++();
    ZipIterator operator++(int);
    reference operator*();
  };

  ZipRange() = delete;

  template <typename Range, typename... Ranges>
  explicit ZipRange(Range &&, Ranges && ...);

  ZipIterator begin();
  ZipIterator end();

  using value_type = typename ZipIterator::value_type;
  using difference_type = typename ZipIterator::difference_type;
  using reference = typename ZipIterator::reference;
  using const_reference = const typename ZipIterator::reference;
  using pointer = typename ZipIterator::pointer;
  using const_pointer = const typename ZipIterator::pointer;
  using iterator = ZipIterator;


private:
  ZipIterator m_Begin;
  ZipIterator m_End;
};

#include "ZipRange.inl.hxx"

#endif // ZIP_RANGE_HXX_
