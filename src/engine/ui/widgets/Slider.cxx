#include "Slider.hxx"
#include "../basics/LOG.hxx"
#include "Settings.hxx"

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
    
    //move all the elements being slid by this slider
    for(auto elem : m_uiElements)
    {
      SDL_Rect r = elem->getUiBaseElementRect();
      elem->setPosition(r.x + curVal - m_maxVal/2, r.y);
    }

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

    //move all the elements being slid by this slider
    for(auto elem : m_uiElements)
    {
      SDL_Rect r = elem->getUiBaseElementRect();
      elem->setPosition(r.x + curVal - m_maxVal/2, r.y);
    }
  }
}

bool Slider::isMouseOver(int x, int y) { return overSliderButton(x, y) || overSliderLine(x, y); }

void Slider::addUiElement(UIElement *elem){
  //add the new element to m_uiElements
  elem->setBasePosition(elem->getUiElementRect().x, elem->getUiElementRect().y);
  m_uiElements.push_back(elem);
  m_maxVal = m_maxVal + Settings::instance().subMenuButtonWidth * 1.5;
};

void Slider::addUiElements(std::vector<UIElement *> newElems) 
{ 
  for(auto elem : newElems)
  {
    addUiElement(elem);
  }
}

void Slider::clearUiElements() 
{
  for(auto elem : m_uiElements)
  {
    elem->setPosition(elem->getUiBaseElementRect().x, elem->getUiBaseElementRect().y);
  }
  m_uiElements.clear(); 
  m_maxVal = 0; 
}