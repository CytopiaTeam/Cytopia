#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "uiElement.hxx"
#include "../basics/timer.hxx"

class Tooltip : public UiElement
{
public:
  Tooltip() : UiElement(0, 0, 0, 0, "", 0, ""){};
  ~Tooltip() = default;

  void setText(const std::string &text);
  virtual void draw();
  void startTimer() { _tooltipTimer.start(); }

private:
  Timer _tooltipTimer;
  std::string _toolTipText;
};

#endif