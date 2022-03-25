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
  dragMode = false;
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
  double ratio = (val - m_minVal) / (double)range;
  int x = ratio * sliderLine.w + sliderLine.x;
  // translate that to position on the slider (For the button)
  sliderButton.x = x - sliderButton.w / 2;
  curVal = val;
}

int Slider::getValue() { return curVal; }

int Slider::getValue(int x)
{
  int range = m_maxVal - m_minVal;
  double ratio = (x - sliderLine.x) / (double)sliderLine.w;
  int val = ratio * range + m_minVal;
  return val;
}

bool Slider::overSliderButton(int x, int y)
{
  return x > sliderButton.x && x < (sliderButton.x + sliderButton.w) && y > sliderButton.y &&
         y < (sliderButton.y + sliderButton.h);
}

bool Slider::overSliderLine(int x, int y)
{
  return x > sliderLine.x && x < (sliderLine.x + sliderLine.w) && y > sliderLine.y && y < (sliderLine.y + sliderLine.h);
}

bool Slider::onMouseButtonDown(const SDL_Event &event)
{
  if (overSliderLine(event.button.x, event.button.y))
  {
    sliderButton.x = event.button.x - sliderButton.w / 2; // sets the middle of the button to where the user clicked
    curVal = getValue(event.button.x);
    return true;
  }
  if (overSliderButton(event.button.x, event.button.y))
  {
    dragMode = true;
    return true;
  }
  return false;
}

bool Slider::onMouseButtonUp(const SDL_Event &event)
{
  dragMode = false;
  return false;
}

void Slider::onMouseMove(const SDL_Event &event)
{
  if (dragMode)
  {
    // button's center will follow mouse
    if (event.motion.x >= sliderLine.x && event.motion.x <= sliderLine.x + sliderLine.w)
    {
      sliderButton.x = event.motion.x - sliderButton.w / 2;
      curVal = getValue(event.motion.x);
    }
  }
}

bool Slider::isMouseOver(int x, int y) { return overSliderButton(x, y) || overSliderLine(x, y); }

void Slider::setPosition(int x, int y) 
{
  sliderLine.x = x;
  sliderLine.y = y + (sliderButton.h / 2) - (m_lineThickness / 2);
  sliderButton.y = y;
}

void Slider::registerCallbackFunction(std::function<void(UIElement* sender)> const& cb) 
{
  //smth
}