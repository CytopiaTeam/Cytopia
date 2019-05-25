#include "text.hxx"

Text::Text(const SDL_Rect &uiElementRect, const std::string &text) : UiElement(uiElementRect) { UiElement::setText(text); }

Text::Text(const std::string &text) { UiElement::setText(text); }
