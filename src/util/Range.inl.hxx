template <typename Iterator>
template <typename Container>
Range<Iterator>::Range(Container&& ctn) : m_Begin(ctn.begin()), m_End(ctn.end())
{ }

template <typename Iterator>
template <typename Container>
Range<Iterator>::Range(const Container&& ctn) : m_Begin(ctn.cbegin()), m_End(ctn.cend())
{ }

template <typename Iterator>
Range<Iterator>::Range(iterator begin, iterator end);

template <typename Iterator>
typename Range<Iterator>::iterator Range<Iterator>::begin()
{
  return m_Begin;
}

template <typename Iterator>
typename Range<Iterator>::iterator Range<Iterator>::end()
{
  return m_End;
}
