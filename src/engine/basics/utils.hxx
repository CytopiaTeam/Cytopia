#ifndef UTILS_HXX_
#define UTILS_HXX_

namespace utils
{
namespace Lib
{
/// Reverse the given iterator - to be used with for range loops
template <class T> class Reverse
{
  T const &elem;

public:
  Reverse(T const &r) noexcept : elem(r) {}

  auto begin() const noexcept { return std::make_reverse_iterator(std::end(elem)); }
  auto end() const noexcept { return std::make_reverse_iterator(std::begin(elem)); }
};
}
/// Helper function for Reverse template because c++11 does not support template deduction
template <class T> auto ReverseIterator(T const &elem) { return Lib::Reverse<T>(elem); }
}

#endif