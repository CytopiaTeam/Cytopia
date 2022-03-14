#include "Tooltip.hxx"

Tooltip::Tooltip()
{
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
  if (!m_active)
  {
    setVisibility(false);
    clockHndl = GameClock::instance().addRealTimeClockTask(
        [this]
        {
          showTooltip();
          return true;
        },
        700ms, 0s);
    m_active = true;
  }
}

void Tooltip::showTooltip() { setVisibility(true); }

void Tooltip::reset()
{
  if (m_active)
  {
    setVisibility(false);
    GameClock::instance().removeClockTask(clockHndl);
    m_active = false;
  }
}
