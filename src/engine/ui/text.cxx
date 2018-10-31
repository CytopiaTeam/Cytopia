#include "text.hxx"

Text::Text(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

Text::Text(const SDL_Rect &uiElementRect, const std::string &text) : UiElement(uiElementRect) { setText(text); }

Text::Text(const std::string &text) : UiElement(SDL_Rect{0, 0, 0, 0}) { setText(text); }