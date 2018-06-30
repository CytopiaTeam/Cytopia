#include <string>

#include "uiElement.hxx"
#include "../basics/resources.hxx"

class Text : public UiElement
{
public:
  Text(int x, int y, const std::string &text, int groupID = 0, int actionID = 0, int parentOfGroup = 0,
       const std::string &tooltipText = "");
  ~Text() = default;
};