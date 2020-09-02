template <typename Iterator>
template <typename Container>
Range<Iterator>::Range(Container &ctn) : m_Begin(ctn.begin()), m_End(ctn.end())
{
}

template <typename Iterator> template <typename Container> Range<Iterator>::Range(const Container &&)
{
  static_assert(sizeof(Container) < 0, "You cannot construct a Range from a temporary container");
}

template <typename Iterator> Range<Iterator>::Range(Iterator begin, Iterator end) : m_Begin(begin), m_End(end) {}

template <typename Iterator> Iterator Range<Iterator>::begin() const { return m_Begin; }

template <typename Iterator> Iterator Range<Iterator>::end() const { return m_End; }

template <typename Iterator1, typename Iterator2> bool operator==(const Range<Iterator1> &r1, const Range<Iterator2> &r2)
{
  Iterator1 it1 = r1.begin();
  Iterator2 it2 = r2.begin();
  Iterator1 end1 = r1.end();
  if (std::distance(it1, end1) != std::distance(it2, r2.end()))
    return false;
  while (it1 != end1)
  {
    if (*it1 != *it2)
      return false;
    ++it1;
    ++it2;
  }
  return true;
}
