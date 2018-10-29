#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "uiElement.hxx"
#include "../basics/timer.hxx"

class Tooltip : public UiElement
{
public:
  Tooltip();
  ~Tooltip() override = default;

  void draw() override;
  void startTimer();
  void showTooltip();
  void reset();

private:
  Timer _tooltipTimer;
  bool _active = false;
};

#endif