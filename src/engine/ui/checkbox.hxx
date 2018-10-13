#ifndef CHECKBOX_HXX_
#define CHECKBOX_HXX_

#include "uiElement.hxx"

class Checkbox : public UiElement
{
public:
  Checkbox(const SDL_Rect &uiElementRect);
  ~Checkbox() override = default;

  SDL_Rect rect;
  void draw() override;

  void onMouseButtonUp(SDL_Event &event) override;
  void onMouseButtonDown(SDL_Event &event) override;
  void onMouseEnter(SDL_Event &event) override;
  void onMouseLeave(SDL_Event &event) override;

private:
  bool checked = false;

  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(const std::string &)> toggleGroupSignal;
};

#endif