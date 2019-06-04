#include "Text.hxx"

Text::Text(const SDL_Rect &uiElementRect, const std::string &text) : UIElement(uiElementRect) { setText(text); }

Text::Text(const std::string &text) : UIElement() { setText(text); }
