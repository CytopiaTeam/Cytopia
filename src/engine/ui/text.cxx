#include "text.hxx"

Text::Text(int x, int y, const std::string &text, int groupID, int actionID, int parentOfGroup, const std::string &tooltipText)
    : UiElement(x, y, text, groupID, actionID, parentOfGroup, tooltipText)
{
}