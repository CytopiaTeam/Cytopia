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

template <typename Iterator> Iterator Range<Iterator>::begin() { return m_Begin; }

template <typename Iterator> Iterator Range<Iterator>::end() { return m_End; }
