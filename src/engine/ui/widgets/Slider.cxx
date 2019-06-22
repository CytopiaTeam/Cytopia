#include "Slider.hxx"

Slider::Slider(const SDL_Rect &uiElementRect) : UIElement(uiElementRect)
{
  sliderLine = uiElementRect;
  sliderLine.x = uiElementRect.h / 2 - m_lineThickness / 2;
  sliderLine.h = m_lineThickness;

  sliderButton = uiElementRect;
  sliderButton.w = 20;
  sliderButton.h = 40;
  sliderButton.y = uiElementRect.y - uiElementRect.h / 2 + m_lineThickness / 2;
  sliderButton.x = uiElementRect.w / 2 - sliderButton.w / 2;
};

void Slider::draw()
{
  drawFrame(sliderLine);
  drawButtonFrame(sliderButton);
}

void Slider::setValue(int val)
{
	int range = m_maxVal - m_minVal;
	// calculate position to set
	// translate that to position on the slider (For the button)
	curVal = val;
}

int Slider::getValue(int x, int y)
{
	return -1;
}
