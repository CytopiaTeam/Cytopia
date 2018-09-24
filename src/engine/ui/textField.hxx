#ifndef TEXTFIELD_HXX_
#define TEXTFIELD_HXX_

#include "uiElement.hxx"
#include "text.hxx"

class TextField : public UiElement
{
public:
  TextField() = default;
  TextField(const SDL_Rect &uiElementRect);

  ~TextField() override = default;

  void addText(std::string text);

  int getSeletectedID(int x, int y);

  std::string getTextFromID(int id);

  SDL_Rect getSize() { return _textFieldRect; };

  void draw() override;

private:
  std::map<int, Text *> _textList;
  SDL_Rect _textFieldRect;
  bool _centerText = true;
};

#endif