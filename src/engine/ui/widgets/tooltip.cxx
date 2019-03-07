#include "tooltip.hxx"

Tooltip::Tooltip()
{
  m_tooltipTimer.registerCallbackFunction(Signal::slot(this, &Tooltip::showTooltip));
  m_tooltipTimer.setTimer(1500);
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
  m_tooltipTimer.start();
  m_active = true;
}

void Tooltip::showTooltip() { setVisibility(true); }

void Tooltip::reset()
{
  if (m_active)
  {
    setVisibility(false);
    m_tooltipTimer.stop();
    m_active = false;
  }
}