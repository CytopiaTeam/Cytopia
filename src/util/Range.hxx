#ifndef RANGE_HXX
#define RANGE_HXX

#include <iterator>
#include "../util/Meta.hxx"

/**
 * @class   Range
 * @brief   A Range is a range between two iterators within a container
 * @tparam  Iterator a type that must satisfy the LegacyIterator named requirement
 * @see     https://en.cppreference.com/w/cpp/named_req/Iterator
 * @example tests/util/Range.cxx
            This is an example of how to use Ranges.
 */
template <typename Iterator> class Range
{
public:
  using iterator = Iterator;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

  /**
   * @brief   Create a Range from a Container
   */
  template <typename Container> Range(Container &ctn);

  /**
   * @brief   Do not use. Will issue an error
   * @details Defined to prevent mistakes
   */
  template <typename Container> Range(const Container &&);

  /**
   * @brief   Create a Range from Iterators
   */
  Range(iterator begin, iterator end);

  iterator begin() const;
  iterator end() const;

private:
  iterator m_Begin;
  iterator m_End;
};

/* Deduction guides for Ranges */
template <typename Container> Range(const Container &) -> Range<typename Container::const_iterator>;

template <typename Container> Range(Container &) -> Range<typename Container::iterator>;

template <typename Iterator> Range(Iterator, Iterator) -> Range<Iterator>;

template <typename Iterator1, typename Iterator2> bool operator==(const Range<Iterator1> &, const Range<Iterator2> &);

#include "Range.inl.hxx"

#endif // RANGE_HXX
