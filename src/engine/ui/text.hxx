#include <string>

#include "uiElement.hxx"
#include "../basics/resources.hxx"

class Text : public UiElement
{
public:
  Text(int x, int y, const std::string &text, const std::string &groupID = "", int actionID = 0,
       const std::string &parentOfGroup = "");
  ~Text() = default;
};