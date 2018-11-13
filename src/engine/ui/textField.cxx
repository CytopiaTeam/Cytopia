#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect)
{
  // initialize height with an offset of 4 so the frame doesn't overlap with the last text element
  // this elemenets height will be adjusted accordingly when the textField is filled.
  _uiElementRect.h = 4;
  _hoverRect = _uiElementRect;
  _hoverRect.x += 4;
  _hoverRect.w -= 8;
}

void TextField::draw()
{
  if (isVisible())
  {
    if (_hoveredID != -1)
    {
      drawSolidRect(_hoverRect, SDL_Color({150, 150, 150}));
    }
    for (auto text : _textVector)
    {
      text->draw();
    }
  }
}

void TextField::addText(const std::string& text)
{
  SDL_Rect textRect = _uiElementRect;

  _textVector.push_back(new Text(text));
  textRect.h = _textVector.back()->getUiElementRect().h; // get height of text after instantiating
  textRect.y = _uiElementRect.y + (((int)_textVector.size() - 1) * textRect.h);

  // center text
  textRect.x = _uiElementRect.x + (_uiElementRect.w / 2 - _textVector.back()->getUiElementRect().w / 2);

  _textElementHeight = textRect.h;

  _uiElementRect.h += _textElementHeight;
  _hoverRect.h = _textElementHeight;

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
    _selectedID = ((_textElementHeight + event.button.y - _uiElementRect.y) / _textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
    if (_selectedID > _textVector.size())
    {
      _selectedID = (int)_textVector.size();
    }
  }
}

void TextField::onMouseMove(const SDL_Event &event)
{
  // TODO: Calculation deviates by 4 pixel in height !
  if (_hoveredID != ((_textElementHeight + event.button.y - _uiElementRect.y) / _textElementHeight) - 1)
  {
    _hoveredID = ((_textElementHeight + event.button.y - _uiElementRect.y) / _textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
    if (_hoveredID > _textVector.size() - 1)
    {
      _hoveredID = (int)_textVector.size() - 1;
    }
    _hoverRect.y = ((_hoveredID)*_textElementHeight) + _uiElementRect.y;
  }
}

void TextField::onMouseLeave(const SDL_Event &event)
{
  // reset the hovering
  _hoveredID = -1;
}