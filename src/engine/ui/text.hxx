#include <string>

#include "uiElement.hxx"
#include "../basics/resources.hxx"


class Text : public UiElement
{
public:
  Text(int x, int y, std::string& text);
  ~Text() = default;

};