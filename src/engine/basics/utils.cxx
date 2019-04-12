#include "utils.hxx"
namespace utils
{
namespace strings
{

void removeSubString(std::string &string, const std::string &stringToRemove)
{
  size_t pos = string.find(stringToRemove);

  if (pos != std::string::npos)
  {
    string.erase(pos, stringToRemove.length());
  }
}

bool endsWith(const std::string &mainStr, const std::string &toMatch)
{
  return mainStr.size() >= toMatch.size() && mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0;
}

bool startsWith(std::string mainStr, std::string toMatch)
{
  return mainStr.size() >= toMatch.size() && mainStr.compare(0, toMatch.size(), toMatch) == 0;
}

} // namespace strings
} // namespace utils