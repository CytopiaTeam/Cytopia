#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect)
{
  // initialize height with zero, it'll be adjusted when the textField is filled.
  _uiElementRect.h = 0;
}

void TextField::addText(std::string text)
{
  _textList.insert(std::make_pair(static_cast<int>(_textList.size()), new Text(text)));

  SDL_Rect textRect = _uiElementRect;

  _textVector.push_back(new Text(text));
  textRect.h = _textVector.back()->getUiElementRect().h; // get height of text after instantiating
  textRect.y = _uiElementRect.y + (((int)_textVector.size() - 1) * textRect.h);

  // center text
  textRect.x = _uiElementRect.x + (_uiElementRect.w / 2 - _textVector.back()->getUiElementRect().w / 2);

  _uiElementRect.h += textRect.h;

  _textVector.back()->setPosition(textRect.x, textRect.y);
}

int TextField::getSeletectedID(int x, int y)
{
  if (!_textList.empty())
  {
    // pick the dimensions of the last element in the map, assuming all texts have the same height
    SDL_Rect currRect = (--_textList.end())->second->getUiElementRect();
    return ((currRect.h + y - _uiElementRect.y) / currRect.h) - 1;
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
    for (auto text : _textVector)
    {
      text->draw();
    }
  }
}
