#include "frame.hxx"

Frame::Frame(const SDL_Rect &uiElementRect) : UiElement(uiElementRect) {}

void Frame::draw() { drawFrame(m_uiElementRect); }