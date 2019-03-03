#ifndef TEXTFIELD_HXX_
#define TEXTFIELD_HXX_

#include "../basics/uiElement.hxx"
#include "text.hxx"

class TextField : public UiElement
{
public:
  TextField() = default;
  TextField(const SDL_Rect &uiElementRect);

  ~TextField() override = default;

  void addText(const std::string &text);

  int getSelectedID() { return _selectedID; };

  int getHoveredID() { return _hoveredID; };

  std::string getTextFromID(int id);

  SDL_Rect getSize() { return _uiElementRect; };

  void draw() override;

  void onMouseButtonUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  bool centerText = true;

  int count() const { return _count; };

private:
  std::vector<Text *> _textVector;

  int _textElementHeight = 0;
  int _selectedID = -1;
  int _hoveredID = -1;

  int _count = 0;
  // a rect is drawn beneath the current text to hover it
  SDL_Rect _hoverRect;
};

#endif