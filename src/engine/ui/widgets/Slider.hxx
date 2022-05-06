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

  /** @brief updates the slider's value
   * @details Sets the value of the slider and updates the
   * sliderButton position accordingly
   * @param val the new value of the slider
   */
  void setValue(int);

  /** @brief gets the current value of the slider
   * @returns the slider's current value
   */
  int getValue() { return m_curVal; };

  /** @brief Checks if the mouse is over the button of the slider
   * @param x,y the current coordinates of the mouse
   * @returns whether or not the mouse is over the button
   */
  bool overSliderButton(int, int);

  /** @brief Checks if the mouses is over the line part of the slider
   * @param x,y the current coordinates of the mouse
   * @returns whether or not the mouse is over the button
   */
  bool overSliderLine(int, int);
  bool onMouseButtonDown(const SDL_Event &) override;
  bool onMouseButtonUp(const SDL_Event &) override;
  void onMouseMove(const SDL_Event &) override;

  /** @brief Checks if the mouse is over either part of the slider
   * @param x,y the current coordinates of the mouse
   * @returns true if mouse is over the line or button, false otherwise
   */
  bool isMouseOver(int, int) override;

  void setPosition(int x, int y) override;

  void registerOnValueChanged(std::function<void(int value)> const &cb);

private:
  /** @brief finds the slider's value based on where the button is on the line
   * @details uses the button's coordinates to calculate the corresponding value
   * @param x the x coordinate of the center of the sliderButton
   * @returns the value of the slider when the center of the button is at given x coordinate
   */
  int valueAtPos(int x);

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
  int m_curVal = 0;
  /// whether or not the button is to follow the mouse
  bool m_dragMode = false;

  // Signal
  Signal::Signal<void(int value)> sliderSignal;
};

#endif