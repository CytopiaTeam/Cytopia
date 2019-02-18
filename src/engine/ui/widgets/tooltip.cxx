#include "tooltip.hxx"

Tooltip::Tooltip()
{
  _tooltipTimer.registerCallbackFunction(Signal::slot(this, &Tooltip::showTooltip));
  _tooltipTimer.setTimer(1500);
  setVisibility(false);
}

void Tooltip::draw()
{
  if (isVisible())
  {
    drawTextFrame();
    renderTexture();
  }
}

void Tooltip::startTimer()
{
  setVisibility(false);
  _tooltipTimer.start();
  _active = true;
}

void Tooltip::showTooltip() { setVisibility(true); }

void Tooltip::reset()
{
  if (_active)
  {
    setVisibility(false);
    _tooltipTimer.stop();
    _active = false;
  }
}