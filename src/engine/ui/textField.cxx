#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

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
  }
  LOG() << "DEBUG our rect: " << currRect.x << ", " << currRect.y << " and size " << currRect.w << ", " << currRect.h;
}

void TextField::draw() 
{
  for (auto text : _textList)
  {
    text.second->draw();
  }
}
