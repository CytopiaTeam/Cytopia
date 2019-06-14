#ifndef SLIDER_HXX_
#define SLIDER_HXX_

#include <string>

#include "../basics/UIElement.hxx"

class Slider : public UIElement
{
public:
  Slider(const SDL_Rect &uiElementRect);

  Slider() = default;
  ~Slider() override = default;

  void draw() override;


private:
  /// Thickness of the slider line
  int m_lineThickness = 6;
  /// coordinates / dimensions of the slider line
  SDL_Rect sliderLine{0, 0, 0, 0};
  /// coordinates / dimensions of the slider Button
  SDL_Rect sliderButton{0, 0, 0, 0};
  /// minimum slider value
  int m_minVal = 0;
  /// maximum slider value
  int m_maxVal = 100;
};

#endif