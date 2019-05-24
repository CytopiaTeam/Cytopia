#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "../basics/uiElement.hxx"
#include "text.hxx"

enum class TextLayout
{
  CENTERED,
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
};
class Button : public UiElement
{
public:
  Button(const SDL_Rect &uiElementRect);
  ~Button() override = default;

  void draw() override;

  void setText(const std::string &text) override;

  bool onMouseButtonUp(const SDL_Event &event) override;
  bool onMouseButtonDown(const SDL_Event &event) override;
  void onMouseEnter(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  void drawImageButtonFrame(bool drawFrame) override;

  void registerCallbackFunction(std::function<void()> const &cb) override;
  void registerCallbackFunction(std::function<void(UiElement *sender)> const &cb) override;
  void registerCallbackFunction(std::function<void(const std::string &, UiElement *sender)> const &cb) override;

  void setPosition(int x, int y) override;
  void centerTextLabel() const;

  bool checkState() const { return m_checkState; };
  void setCheckState(bool state);

private:
  SDL_Rect m_frameRect;

  std::unique_ptr<Text> m_buttonLabel;

  bool m_isMouseButtonDown = false;

  bool m_drawFrame = false;
  bool m_checkState = false;

  // Signals
  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(UiElement *sender)> clickSignalSender;
  Signal::Signal<void(const std::string &, UiElement *sender)> clickSignalString;
};

#endif