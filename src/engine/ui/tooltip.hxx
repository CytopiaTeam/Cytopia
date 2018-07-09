#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "uiElement.hxx"
#include "../basics/timer.hxx"

class Tooltip : public UiElement
{
public:
  Tooltip() : UiElement(SDL_Rect{0, 0, 0, 0}){};
  ~Tooltip() override = default;

  void setText(const std::string &text);
  void draw() override;
  void startTimer() { _tooltipTimer.start(); }

private:
  Timer _tooltipTimer;
  std::string _toolTipText;
};

#endif