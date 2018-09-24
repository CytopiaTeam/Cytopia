#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), _textFieldRect(uiElementRect)
{
  // initialize height with zero, it'll be adjusted when the textField is filled.
  _textFieldRect.h = 0;
}

void TextField::addText(std::string text)
{
  _textList.insert(std::make_pair(static_cast<int>(_textList.size()), new Text(text)));

  SDL_Rect currRect;

  if (!_textList.empty())
  {
    currRect = (--_textList.end())->second->getUiElementRect();

    int xPos;
    if (_centerText)
    {
      xPos = _textFieldRect.x + (_textFieldRect.w / 2) + (currRect.x - currRect.w / 2);
    }
    else
    {
      xPos = _textFieldRect.x;
    }
    int yPos = _textFieldRect.y + (currRect.h * (static_cast<int>(_textList.size()) - 1));

    (--_textList.end())->second->setTextPosition(xPos, yPos);

    _textFieldRect.h += (--_textList.end())->second->getUiElementRect().h;
  }
}

int TextField::getSeletectedID(int x, int y)
{
  if (!_textList.empty())
  {
    // pick the dimensions of the last element in the map, assuming all texts have the same height
    SDL_Rect currRect = (--_textList.end())->second->getUiElementRect();
    return ((currRect.h + y - _textFieldRect.y) / currRect.h) - 1;
  }

  // calculate clicked position in combobox selection from given coordinates;
  return -1;
}

std::string TextField::getTextFromID(int id)
{
  auto result = _textList.find(id);
  if (result != _textList.end())
  {
    return result->second->getText();
  }
  return "";
}

void TextField::draw()
{
  if (isVisible())
  {
    for (auto text : _textList)
    {
      text.second->draw();
    }
  }
}
