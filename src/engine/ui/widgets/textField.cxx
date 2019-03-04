#include "textField.hxx"
#include "text.hxx"

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
    if (hoveredID != -1)
    {
      drawSolidRect(_hoverRect, SDL_Color({150, 150, 150}));
    }
    for (auto text : _textVector)
    {
      text->draw();
    }
  }
}

void TextField::addText(const std::string &text)
{
  SDL_Rect textRect = _uiElementRect;

  _textVector.push_back(new Text(text));
  textRect.h = _textVector.back()->getUiElementRect().h; // get height of text after instantiating
  textRect.y = (_uiElementRect.y + _count - 1) * textRect.h;

  // center text
  if (centerText)
  {
    textRect.x = _uiElementRect.x + (_uiElementRect.w / 2 - _textVector.back()->getUiElementRect().w / 2);
  }
  else
  {
    textRect.x = _uiElementRect.x;
  }

  _textElementHeight = textRect.h;

  _uiElementRect.h += _textElementHeight;
  _hoverRect.h = _textElementHeight;

  _textVector.back()->setPosition(textRect.x, textRect.y);
  _count = static_cast<int>(_textVector.size());
}

std::string TextField::getTextFromID(int id)
{
  if (id < _count)
  {
    return _textVector[id]->getUiElementData().text;
  }
  return "";
}

void TextField::onMouseButtonUp(const SDL_Event &event)
{
  if (!_textVector.empty())
  {
    selectedID = ((_textElementHeight + event.button.y - _uiElementRect.y) / _textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked

    if (selectedID >= _count)
    {
      selectedID = _count - 1;
    }
  }
}

void TextField::onMouseMove(const SDL_Event &event)
{
  // TODO: Calculation deviates by 4 pixel in height because of the frame
  if (hoveredID != ((_textElementHeight + event.button.y - _uiElementRect.y) / _textElementHeight) - 1)
  {
    hoveredID = ((_textElementHeight + event.button.y - _uiElementRect.y) / _textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
    if (hoveredID >= _count)
    {
      hoveredID = _count - 1;
    }
    _hoverRect.y = ((hoveredID)*_textElementHeight) + _uiElementRect.y;
  }
}

void TextField::onMouseLeave(const SDL_Event &)
{
  // reset the hovering
  hoveredID = -1;
}