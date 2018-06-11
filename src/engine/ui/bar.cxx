#include "uiElement.hxx"

class Bar : public UiElement
{
public:
  Bar(int x, int y, std::string& text);
  ~Bar() = default;

};