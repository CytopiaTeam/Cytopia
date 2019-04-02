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
  if (mainStr.size() >= toMatch.size() && mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
    return true;
  else
    return false;
}

bool startsWith(std::string mainStr, std::string toMatch)
{
  // std::string::find returns 0 if toMatch is found at starting
  if (mainStr.find(toMatch) == 0)
    return true;
  else
    return false;
}

} // namespace strings
} // namespace utils