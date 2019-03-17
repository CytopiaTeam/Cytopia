#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "../basics/uiElement.hxx"
#include "text.hxx"

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
  void registerCallbackFunction(std::function<void(const std::string &)> const &cb) override;

  const bool checkState() { return m_checkState; };
  void setCheckState(bool state);
private:
  SDL_Rect m_rect;

  std::unique_ptr<Text> m_buttonLabel;

  bool m_isMouseButtonDown = false;

  bool m_drawFrame = false;
  bool m_checkState = false;

  // Signals
  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(const std::string &)> clickSignalString;
};

#endif