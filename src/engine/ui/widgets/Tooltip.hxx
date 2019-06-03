#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "../basics/UIElement.hxx"
#include "Text.hxx"
#include "../../basics/Timer.hxx"

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
  Timer m_tooltipTimer;
  bool m_active = false;
};

#endif