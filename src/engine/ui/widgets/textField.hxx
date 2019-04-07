#ifndef TEXTFIELD_HXX_
#define TEXTFIELD_HXX_

#include "../basics/uiElement.hxx"
#include "text.hxx"

enum class TextFieldAlignment
{
  LEFT,
  RIGHT,
  CENTERED
};

class TextField : public UiElement
{
public:
  TextField() = default;
  TextField(const SDL_Rect &uiElementRect);

  ~TextField() override = default;

  void addText(const std::string &text);

  std::string getTextFromID(int id) const;

  SDL_Rect getSize() const { return m_uiElementRect; };

  void setPosition(int x, int y) override;

  void draw() override;

  bool onMouseButtonUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  bool centerText = true;

  int selectedID = -1;
  int hoveredID = -1;
  int count = 0;

  TextFieldAlignment textAlignment = TextFieldAlignment::CENTERED;

private:
  std::vector<Text *> m_textElements;

  int m_textElementHeight = 0;

  // a rect is drawn beneath the current text to hover it
  SDL_Rect m_highlightingRect;
};

#endif