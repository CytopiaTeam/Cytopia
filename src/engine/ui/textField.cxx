#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect)
{
  // initialize height with an offset of 4 so the frame doesn't overlap with the last text element
  // this elemenets height will be adjusted accordingly when the textField is filled.
  _uiElementRect.h = 4;
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

void TextField::addText(std::string text)
{
  SDL_Rect textRect = _uiElementRect;

  _textVector.push_back(new Text(text));
  textRect.h = _textVector.back()->getUiElementRect().h; // get height of text after instantiating
  textRect.y = _uiElementRect.y + (((int)_textVector.size() - 1) * textRect.h);

  // center text
  textRect.x = _uiElementRect.x + (_uiElementRect.w / 2 - _textVector.back()->getUiElementRect().w / 2);

  _uiElementRect.h += textRect.h;

  _textVector.back()->setPosition(textRect.x, textRect.y);
}

std::string TextField::getTextFromID(int id)
{
  if (id < _textVector.size())
  {
    return _textVector[id]->getText();
  }
  return "";
}

void TextField::onMouseButtonUp(const SDL_Event &event)
{
  if (!_textVector.empty())
  {
    int height = _textVector.back()->getUiElementRect().h;
    _selectedID = ((height + event.button.y - _uiElementRect.y) / height) - 1;
  }
}