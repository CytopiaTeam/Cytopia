#ifndef CHECKBOX_HXX_
#define CHECKBOX_HXX_

#include "../basics/UIElement.hxx"

class Checkbox : public UIElement
{
public:
  Checkbox(const SDL_Rect &uiElementRect);
  ~Checkbox() override = default;

  void draw() override;

  bool onMouseButtonUp(const SDL_Event &event) override;
  bool onMouseButtonDown(const SDL_Event &event) override;
  void onMouseEnter(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

private:
  bool m_isMouseButtonDown = false;

  // Signals
  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(const std::string &)> toggleGroupSignal;
};

#endif