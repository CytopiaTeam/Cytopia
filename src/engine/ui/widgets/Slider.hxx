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

  /** @brief changes the slider position based on the new value given
   * calculates the slider's position corresponding to the given value
   * @param val the new value of the slider
   */
  void setValue(int);

  /** @brief finds the slider's value based on where the button is on the line
   * uses the button's coordinates to calculate the corresponding value
   * @param x the x coordinate of the center of the sliderButton
   * @returns the current value of the slider
   */
  int getValue(int);

  /** @brief Checks if the mouse is over the button of the slider
   * checks if the mouse is over the sliderButton
   * @param x,y the current coordinates of the mouse
   * @returns whether or not the mouse is over the button
   */
  bool overSliderButton(int, int);

  /** @brief Checks if the mouses is over the line part of the slider
   * checks if the mouse is over the sliderLine
   * @param x,y the current coordinates of the mouse
   * @returns whether or not the mouse is over the button
   */
  bool overSliderLine(int, int);
  bool onMouseButtonDown(const SDL_Event &) override;
  bool onMouseButtonUp(const SDL_Event &) override;
  void onMouseMove(const SDL_Event &) override;

  /** @brief Only overridden to enable dragging functionality
   * this was only redeclared and redefined because the regular implementation 
   * would not work for the Slider widget due to it being composed of two rectangles, 
   * whereas other UIElements are composed of one.
   * @param x,y the current coordinates of the mouse
   * @returns always returns true for the Slider widget only
   */
  bool isMouseOver(int, int) override;

  /** @brief add uiElements to be controlled by this slider
   * @param add uiElements to the vector of elements that will be controlled by this slider
   */
  void addUiElement(UIElement *elem);
  void addUiElements(std::vector<UIElement *>);
  void clearUiElements() {m_uiElements.clear(); scrollRectSet = false; };


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
  /// whether or not the button is to follow the mouse
  bool dragMode;

  // the elements to scroll
  std::vector<UIElement *> m_uiElements;
  // the rect that contains all the scrollable elements
  SDL_Rect scrollRect;
  bool scrollRectSet = false;
  int lastVal;
};

#endif