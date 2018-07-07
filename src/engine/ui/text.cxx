#include "text.hxx"

Text::Text(int x, int y, const std::string &text, const std::string &groupID, int actionID, const std::string &parentOfGroup)
    : UiElement(x, y, text, groupID, actionID, parentOfGroup)
{
}