#include "Slider.hxx"

Slider::Slider(const SDL_Rect &uiElementRect) : UIElement(uiElementRect)
{
  // testing width is 200, pos_x is 400, y is 520, height is 40
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
  // based on where the button is on the line, find the corresponding value

  return -1;
}

bool Slider::onMouseButtonDown(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {
    printf("click\n");
  }
  else
  {
    printf("else\n");
  }
  return false;
}

bool Slider::onMouseButtonUp(const SDL_Event &event)
{
  printf("unclick\n");
  return false;
}
