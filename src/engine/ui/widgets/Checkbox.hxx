#ifndef CHECKBOX_HXX_
#define CHECKBOX_HXX_

#include "../basics/UIElement.hxx"

/**
 * @brief A Checkbox GUI Widget
 */
class Checkbox : public UIElement
{
public:
  Checkbox(const SDL_Rect &uiElementRect);
  ~Checkbox() override = default;

  void draw() override;

  bool onMouseButtonUp(const SDL_Event &event) override;
  bool onMouseButtonDown(const SDL_Event &event) override;
  void onMouseEnter(const SDL_Event &event) override {};
  void onMouseLeave(const SDL_Event &event) override {};

  bool getCheckState() { return m_checkState; };
  void setCheckState(bool checkState);

  void registerCallbackFunction(std::function<void()> const &cb) override { clickSignal.connect(cb); };
  void registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) override { clickSignalSender.connect(cb); };

private:
  bool m_isMouseButtonDown = false;
  bool m_checkState = false;
  // Signals
  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(UIElement *sender)> clickSignalSender;
  Signal::Signal<void(const std::string &)> toggleGroupSignal;
};

#endif