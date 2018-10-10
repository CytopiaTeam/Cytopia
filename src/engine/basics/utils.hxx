namespace utils
{

namespace Lib
{
template <class T> class Reverse
{
  T const &elem;

public:
  Reverse(T const &r) noexcept : elem(r) {}

  auto begin() const noexcept
  {
    using std::end;
    return std::make_reverse_iterator(end(elem));
  }
  auto end() const noexcept
  {
    using std::begin;
    return std::make_reverse_iterator(begin(elem));
  }
};
}
/// Helper function for Reverse template because c++11 does not support template deduction
template <class T> auto ReverseIterator(T const &elem) { return Lib::Reverse<T>(elem); }

/// Reverse the given iterator - to be used with for range loops
}
