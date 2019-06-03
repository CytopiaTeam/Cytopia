#ifndef TEXTFIELD_HXX_
#define TEXTFIELD_HXX_

#include "../basics/UIElement.hxx"
#include "Text.hxx"

enum class TextFieldAlignment
{
  LEFT,
  RIGHT,
  CENTERED
};

class TextField : public UIElement
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

  size_t count() const { return m_count; };
  bool centerText = true;

  int selectedID = -1;
  int hoveredID = -1;

  TextFieldAlignment textAlignment = TextFieldAlignment::CENTERED;

  void clear() { m_textElements.clear(); }

private:
  size_t m_count = 0;

  std::vector<std::unique_ptr<Text>> m_textElements;

  int m_textElementHeight = 0;

  // a rect is drawn beneath the current text to hover it
  SDL_Rect m_highlightingRect;
};

#endif