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

  int getHoveredID() { return _hoveredID; };

  std::string getTextFromID(int id);

  SDL_Rect getSize() { return _uiElementRect; };

  void draw() override;

  void onMouseButtonUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

private:
  std::vector<Text *> _textVector;

  bool _centerText = true;

  int _textElementHeight = 0;
  int _selectedID = -1;
  int _hoveredID = -1;
  // a rect is drawn beneath the current text to hover it
  SDL_Rect _hoverRect;
};

#endif