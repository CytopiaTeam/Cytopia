#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "uiElement.hxx"
#include "../basics/timer.hxx"

class Tooltip : public UiElement
{
public:
  //Tooltip();
  Tooltip::Tooltip();

  ~Tooltip() = default;

  void setText(const std::string &text);
  virtual void draw();
  void startTimer();

  Timer tooltipTimer;

private:
  std::string _toolTipText;
};

#endif