#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

void TextField::addText(std::string text)
{
  _textList.insert(std::make_pair(static_cast<int>(_textList.size()), new Text(_textFieldRect, text)));
}

void TextField::draw() 
{
  for (auto text : _textList)
  {
    text.second->draw();
  }
}
