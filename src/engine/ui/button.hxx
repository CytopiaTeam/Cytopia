#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "uiElement.hxx"

class Button : public UiElement
{
public:
  Button(const SDL_Rect &uiElementRect);
  ~Button() override = default;

  void draw() override;

  void onMouseButtonUp(SDL_Event &event) override;
  void onMouseButtonDown(SDL_Event &event) override;
  void onMouseEnter(SDL_Event &event) override;
  void onMouseLeave(SDL_Event &event) override;

  void registerFunction(std::function<void()> const &cb) override;
  void registerToggleUIFunction(std::function<void(const std::string &)> const &cb) override;

private:
  SDL_Rect rect;

  bool _isPressed = false;
  bool _isMouseOver = false;

  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(const std::string &)> toggleGroupSignal;
};

#endif