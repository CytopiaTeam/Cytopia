#include "Frame.hxx"

Frame::Frame(const SDL_Rect &uiElementRect) : UIElement(uiElementRect) {}

void Frame::draw() { drawFrame(m_uiElementRect); }
