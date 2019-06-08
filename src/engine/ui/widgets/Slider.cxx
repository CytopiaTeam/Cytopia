#include "Slider.hxx"

Slider::Slider(const SDL_Rect &uiElementRect) : UIElement(uiElementRect){};

void Slider::draw()
{
  drawFrame(m_uiElementRect);
}
