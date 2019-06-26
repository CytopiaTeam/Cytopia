#ifndef SLIDER_HXX_
#define SLIDER_HXX_

#include <string>

#include "../basics/UIElement.hxx"

/**
 * @brief A Slider GUI Widget
 */
class Slider : public UIElement
{
public:
  Slider(const SDL_Rect &uiElementRect);

  Slider() = default;
  ~Slider() override = default;

  void draw() override;
  void setValue(int);
  int getValue(int, int);

  /** @brief Checks if the mouse is over the button of the slider
   * checks if the mouse is over the sliderButton
   * @param x, y the current coordinates of the mouse
   * @returns whether or not the mouse is over the button
   */
  bool overSliderButton(int, int);
  bool onMouseButtonDown(const SDL_Event &) override;
  bool onMouseButtonUp(const SDL_Event &) override;

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
  /// current slider value
  int curVal;
};

#endif