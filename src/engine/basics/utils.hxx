#ifndef UTILS_HXX_
#define UTILS_HXX_

#include <iterator>
#include <string>

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
} // namespace Lib
/// Helper function for Reverse template because c++11 does not support template deduction
template <class T> auto ReverseIterator(T const &elem) { return Lib::Reverse<T>(elem); }

namespace strings
{

/** @brief Erase first occurrence of given substring from main string.
* @param string main string to modify
* @param stringToRemove substring to remove
*/
void removeSubString(std::string &string, const std::string &stringToRemove);

/** @brief checks if a string ends with a certain substring
* @param mainStr string to scan through
* @param toMatch substring to search for
* @returns if substring is at the end of the main string
*/
bool endsWith(const std::string &mainStr, const std::string &toMatch);

/** @brief checks if a string starts with a certain substring
* @param mainStr string to scan through
* @param toMatch substring to search for
* @returns if substring is at the start of the main string
*/
bool startsWith(const std::string &mainStr, const std::string &toMatch);

} // namespace strings
} // namespace utils

#endif