#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "../basics/UIElement.hxx"
#include "Text.hxx"

enum class TextLayout
{
  CENTERED,
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
};

/**
 * @brief A Button GUI Widget
 */
class Button : public UIElement
{
public:
  Button(const SDL_Rect &uiElementRect);
  ~Button() override = default;

  void draw() override;

  /** @brief Sets the button's text
   * @details sets the button's text to the string passed as an argument
   * @param text the text to be displayed on the button
   */
  void setText(const std::string &text);

  bool onMouseButtonUp(const SDL_Event &event) override;
  bool onMouseButtonDown(const SDL_Event &event) override;
  void onMouseEnter(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  void drawImageButtonFrame(bool drawFrame);

  void registerCallbackFunction(std::function<void()> const &cb) override;
  void registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) override;
  void registerCallbackFunction(std::function<void(const std::string &, UIElement *sender)> const &cb) override;

  void setPosition(int x, int y) override;
  void centerTextLabel() const;

  bool checkState() const { return m_checkState; };
  void setCheckState(bool state);

  bool isToggleButton = false; ///< specifies if this is a toggle button

private:
  SDL_Rect m_frameRect;

  /// a pointer to the button's text
  std::unique_ptr<Text> m_buttonLabel;

  bool m_isMouseButtonDown = false;

  bool m_drawFrame = false;
  bool m_checkState = false;

  // Signals
  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(UIElement *sender)> clickSignalSender;
  Signal::Signal<void(const std::string &, UIElement *sender)> clickSignalString;
};

#endif