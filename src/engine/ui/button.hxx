#ifndef BUTTON_HXX_
#define BUTTON_HXX_

#include "uiElement.hxx"

class Button : public UiElement
{
public:
  Button(const SDL_Rect &uiElementRect);
  ~Button() override = default;

  void draw() override;

  void onMouseButtonUp(const SDL_Event &event) override;
  void onMouseButtonDown(const SDL_Event &event) override;
  void onMouseEnter(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  void registerCallbackFunction(std::function<void()> const &cb) override;
  void registerToggleUIFunction(std::function<void(const std::string &)> const &cb) override;

private:
  SDL_Rect _rect;

  bool _isMouseButtonDown = false;

  Signal::Signal<void()> clickSignal;
  Signal::Signal<void(const std::string &)> toggleGroupSignal;
};

#endif