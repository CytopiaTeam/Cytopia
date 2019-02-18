#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "../basics/uiElement.hxx"
#include "text.hxx"
#include "../../basics/timer.hxx"

/** Tooltip class
  * Tooltip is derived from Text, and adds some extensions to it. It is set to visible after 
  * the timer times out.
  * @note There should only be one tooltip object, managed by the UIManager
  * @see UIManager#startTooltip
*/
class Tooltip : public Text
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