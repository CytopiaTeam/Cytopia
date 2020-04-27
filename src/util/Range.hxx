#ifndef RANGE_HXX
#define RANGE_HXX

#include <iterator>
#include "../util/Meta.hxx"

template <typename Type, typename Enable = void>
class Range
{
  static_assert(sizeof(Type) < 0, "In Range<Type>, Type was neither an Iterator nor an integral type");
};

template <typename Iterator>
class Range<Iterator, 
      std::void_t<typename std::iterator_traits<Iterator>::iterator_category>>
{
public:
  using iterator = Iterator;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

  template <typename Container>
  Range(Container&& ctn) :
    m_Begin(ctn.begin()), m_End(ctn.end()) { }
  
  template <typename Container>
  Range(const Container&& ctn) :
    m_Begin(ctn.cbegin()), m_End(ctn.cend()) { }
  
  Range(iterator begin, iterator end) : 
    m_Begin(begin), m_End(end) { }
  
  iterator begin() { return m_Begin; }
  
  iterator end() { return m_End; }
private:
  iterator m_Begin;
  iterator m_End;
};

#include "Range.inl.hxx"

#endif // RANGE_HXX
