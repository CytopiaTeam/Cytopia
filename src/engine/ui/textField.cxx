#include "textField.hxx"
#include "text.hxx"

//TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

void TextField::addText(std::string text)
{

  _texts.push_back(new Text (text));
}

void TextField::draw() 
{
  for (auto text : _texts)
  {
    text->draw();
  }
}
