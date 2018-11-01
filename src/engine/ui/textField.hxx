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

  int getSeletectedID(int x, int y) { return _selectedID; };

  std::string getTextFromID(int id);

  SDL_Rect getSize() { return _uiElementRect; };

  void draw() override;

  void onMouseButtonUp(const SDL_Event &event) override;

private:
  std::vector<Text *> _textVector;

  bool _centerText = true;

  int _selectedID = -1;
};

#endif